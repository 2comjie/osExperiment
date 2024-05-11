#include <stdio.h>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#define JOBFCFS 1
#define JOBSJF 2
#define JOBHRRN 3

#pragma region 结构体变量声明
// 作业
struct JOB
{
    int j_id;           // 作业标识
    int sub_time;       // 作业提交的时间
    int enter_mem_time; // 作业进入内存的时间
    int finish_time;    // 作业完成的时间
    int exit_time;      // 作业退出的时间
    int run_time;       // 作业预计运行时间
    int start_time;     // 对应进程开始运行时间
    int proty;

    bool subed; // 作业是否已经提交
    JOB(int j_id, int sub_time, int run_time, int proty)
    {
        this->j_id = j_id;
        this->sub_time = sub_time;
        this->run_time = run_time;
        this->proty = proty;
        this->enter_mem_time = -1;
        this->subed = false;
        this->start_time = -1;
    }
};

// 进程结构体
struct PROC
{
    int p_id;     // 进程标识
    int cpu_time; // 已经运行的时间
    JOB *job;     // 对应的作业

    PROC(JOB *job, int p_id)
    {
        this->job = job;
        this->p_id = p_id;
        this->cpu_time = 0;
    }
};

struct CPU
{
    PROC *proc;
    CPU() { this->proc = nullptr; }
};

int N = 0;                           // N道处理系统
int curtime = 0;                     // 当前系统运行时间
std::set<JOB *> total_jobset;        // 总的作业集合
std::set<JOB *> wait_schdule_jobset; // 当前时间正在等待被调度的作业
std::set<PROC *> ready_procset;      // 当前内存中的就绪的进程集合
int finish_jobs = 0;                 // 当前已经完成的任务数
CPU *cpu = new CPU();                // 当前CPU
int job_schdule_type = 0;

int calue_turnaround_time(JOB *JOB)
{
    return JOB->finish_time - JOB->sub_time;
}

// 计算带权周转时间
double calue_weighted_turnaround_time(JOB *JOB)
{
    return calue_turnaround_time(JOB) * 1.0 / JOB->run_time;
}

void job_sub()
{
    for (auto it = total_jobset.begin(); it != total_jobset.end(); it++)
        if (!(*it)->subed && curtime >= (*it)->sub_time)
        {
            wait_schdule_jobset.insert(*it);
            (*it)->subed = true;
        }
}

int total_proc()
{
    return cpu->proc == nullptr ? ready_procset.size() : ready_procset.size() + 1;
}
#pragma endregion

#pragma region 作业调度模块
// 先来先服务
JOB *JOB_FCFS()
{
    int min = 0xffff;
    JOB *job = nullptr;
    auto it = wait_schdule_jobset.begin();
    while (it != wait_schdule_jobset.end())
    {
        if (min > (*it)->sub_time)
        {
            min = (*it)->sub_time;
            job = *it;
        }
        it++;
    }
    return job;
}

// 短作业优先
JOB *JOB_SJF()
{
    int min = 0xffff;
    JOB *job = nullptr;
    auto it = wait_schdule_jobset.begin();
    while (it != wait_schdule_jobset.end())
    {
        if (min > (*it)->run_time)
        {
            min = (*it)->run_time;
            job = *it;
        }
        it++;
    }
    return job;
}

// 最高响应比
JOB *JOB_HRRN()
{
    double max = 0;
    JOB *job = nullptr;
    auto it = wait_schdule_jobset.begin();
    while (it != wait_schdule_jobset.end())
    {
        double R = (curtime - (*it)->sub_time + (*it)->run_time) * 1.0 / (*it)->run_time;
        if (R > max)
        {
            R = max;
            job = *it;
        }
        it++;
    }
    return job;
}

void job_schdule()
{
    while (total_proc() < N && !wait_schdule_jobset.empty())
    {
        JOB *job = nullptr;
        if (job_schdule_type == JOBFCFS)
            job = JOB_FCFS();
        else if (job_schdule_type == JOBSJF)
            job = JOB_SJF();
        else if (job_schdule_type == JOBHRRN)
            job = JOB_HRRN();
        else
        {
            printf("job_schdule \n");
            exit(1);
        }
        job->enter_mem_time = curtime;
        wait_schdule_jobset.erase(job);
        ready_procset.insert(new PROC(job, job->j_id));
    }
}
#pragma endregion

#pragma region 进程调度模块
// 优先级调度
PROC *PROC_HPF()
{
    int max = 0;
}

void proc_schdule()
{
    if (cpu->proc != nullptr)
    {
        if (cpu->proc->cpu_time >= cpu->proc->job->run_time)
        {
            cpu->proc->job->finish_time = curtime;
            cpu->proc->job->exit_time = curtime;
            finish_jobs++;
            delete cpu->proc;
            cpu->proc = nullptr;
            job_schdule();
        }
    }

    if (cpu->proc == nullptr)
    {
        int min = 0xffff;
        PROC *proc = nullptr;
        for (auto it = ready_procset.begin(); it != ready_procset.end(); it++)
            if (min > (*it)->job->enter_mem_time)
            {
                min = (*it)->job->enter_mem_time;
                proc = *it;
            }
        {
            ready_procset.erase(proc);
            cpu->proc = proc;
            if (proc->job->start_time == -1)
                proc->job->start_time = curtime;
        }
    }
}
#pragma endregion

#pragma region 测试函数
void update()
{
    while (finish_jobs < total_jobset.size())
    {
        // 提交作业
        job_sub();
        // 作业调度
        job_schdule();
        // 进程调度
        proc_schdule();
        if (cpu->proc != nullptr)
            cpu->proc->cpu_time++;
        curtime++;
    }
}

void show()
{
    double sum1 = 0.0, sum2 = 0.0;
    printf("作业\t\t提交\t\t运行\t\t优先\t\t进入内存\t开始\t\t结束\t\t周转\t\t带权\n");
    for (auto it = total_jobset.begin(); it != total_jobset.end(); it++)
    {
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%lf\n", (*it)->j_id,
               (*it)->sub_time, (*it)->run_time, (*it)->proty, (*it)->enter_mem_time,
               (*it)->start_time, (*it)->finish_time, calue_turnaround_time(*it),
               calue_weighted_turnaround_time(*it));
        sum1 += calue_turnaround_time(*it);
        sum2 += calue_weighted_turnaround_time(*it);
    }
    printf("平均周转时间: %lf, 平均带权周转时间: %lf \n", sum1 / total_jobset.size(), sum2 / total_jobset.size());
}

void init()
{
    std::ifstream file("test.txt");
    std::string line;

    if (!file.is_open())
    {
        std::cerr << "test.txt" << std::endl;
        exit(1);
    }
    getline(file, line);
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (std::getline(iss, token, ','))
        {
            tokens.push_back(token);
        }

        int proty = std::stoi(tokens.back());
        tokens.pop_back();
        int run = std::stoi(tokens.back());
        tokens.pop_back();
        int sub = std::stoi(tokens.back());
        tokens.pop_back();
        int j_id = std::stoi(tokens.back());
        tokens.pop_back();

        JOB *job = new JOB(j_id, sub, run, proty);
        total_jobset.insert(job);
    }

    file.close();
}

int main()
{
    std::cout << "请输入作业调度算法类型" << std::endl;
    std::cout << "1.FCFS 2.SJF 3.HRRN" << std::endl;
    std::cin >> job_schdule_type;
    std::cout << "请输入N道批处理系统: ";
    std::cin >> N;
    init();
    update();
    show();
    return 0;
}
#pragma endregion