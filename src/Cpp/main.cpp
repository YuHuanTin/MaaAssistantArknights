#include "AsstCaller.h"

#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <string>
#include <thread>
#include <print>

// 2025年7月11日获取
enum AsstMsg : int32_t
{
    /* Global Info */
    InternalError     = 0,           // 内部错误
    InitFailed        = 1,           // 初始化失败
    ConnectionInfo    = 2,           // 连接相关信息
    AllTasksCompleted = 3,           // 全部任务完成
    AsyncCallInfo     = 4,           // 外部异步调用信息
    Destroyed         = 5,           // 实例已销毁

    /* TaskChain Info */
    TaskChainError     = 10000,      // 任务链执行/识别错误
    TaskChainStart     = 10001,      // 任务链开始
    TaskChainCompleted = 10002,      // 任务链完成
    TaskChainExtraInfo = 10003,      // 任务链额外信息
    TaskChainStopped   = 10004,      // 任务链手动停止

    /* SubTask Info */
    SubTaskError      = 20000,       // 原子任务执行/识别错误
    SubTaskStart      = 20001,       // 原子任务开始
    SubTaskCompleted  = 20002,       // 原子任务完成
    SubTaskExtraInfo  = 20003,       // 原子任务额外信息
    SubTaskStopped    = 20004,       // 原子任务手动停止
};

void ASST_CALL idk(AsstMsgId msg, const char* details_json, void* custom_arg) {
    std::string strDetails = details_json;
    if (msg == AsstMsg::ConnectionInfo && strDetails.find("ScreencapCost") != std::string::npos) {
        return;
    }
    std::println("\nidk: {}, {}", msg, details_json);
}

int main([[maybe_unused]] int argc, char** argv)
{
    const auto cur_path = std::filesystem::path(argv[0]).parent_path();

    // 可以将日志、调试图片等存到别的目录下，需要在最一开始调用。不调用默认保存到资源同目录
    // AsstSetUserDir(cur_path.c_str());

    // 这里默认读取的是可执行文件同目录下 resource 文件夹里的资源
    if (!AsstLoadResource(cur_path.string().c_str())) {
        std::cerr << "-------- load resource failed: official --------" << std::endl;
        return -1;
    }

#ifdef ASST_DEBUG
    if (argc > 1) {
        const std::string arg(argv[1]);

        if (arg == "Official") {
            std::cout << "Official type detected, using default resources." << std::endl;
        }
        else {
            std::cout << "load overseas_type: " << arg << std::endl;

            const auto overseas_path = cur_path / "resource" / "global" / arg;
            if (!AsstLoadResource(overseas_path.string().c_str())) {
                std::cerr << "-------- load resource failed: " << arg << " --------" << std::endl;
                return -1;
            }
        }
    }
#endif

    auto ptr = AsstCreateEx(idk, nullptr);
    if (ptr == nullptr) {
        std::cerr << "create failed" << std::endl;
        return -1;
    }

#ifdef SMOKE_TESTING
    std::cout << "Ended early for smoke testing." << std::endl;
    return 0;
#endif

#ifndef ASST_DEBUG
    AsstAsyncConnect(ptr, "adb", "127.0.0.1:5555", nullptr, true);
    
#else
    // always success if pass "DEBUG"
    // AsstAsyncConnect(ptr, "D:\\Program Files\\Netease\\MuMu\\nx_main\\adb.exe", "127.0.0.1:16384", "DEBUG", true);
#endif
    AsstAsyncConnect(ptr, "\"D:\\Program Files\\Netease\\MuMu\\nx_main\\adb.exe\"", "127.0.0.1:5555", nullptr, true);
    if (!AsstConnected(ptr)) {
        std::cerr << "connect failed" << std::endl;
        AsstDestroy(ptr);
        ptr = nullptr;

        return -1;
    }
    std::println("connect success");

#ifndef ASST_DEBUG

    /* 详细参数可参考 docs / 集成文档.md */
    /*AsstAppendTask(ptr, "StartUp", nullptr);

    AsstAppendTask(ptr, "Fight", R"(
    {
        "stage": "1-7"
    }
    )");

    AsstAppendTask(ptr, "Recruit", R"(
    {
        "select":[4],
        "confirm":[3,4],
        "times":4
    }
    )");

    AsstAppendTask(ptr, "Infrast", R"(
    {
        "facility": ["Mfg", "Trade", "Power", "Control", "Reception", "Office", "Dorm"],
        "drones": "Money"
    }
    )");

    AsstAppendTask(ptr, "Mall", R"(
    {
        "shopping": true,
        "buy_first": [
            "许可"
        ],
        "black_list": [
            "家具",
            "碳"
        ]
    }
    )");

    AsstAppendTask(ptr, "Award", R"(
    {
        "award": true,
        "mail": true,
        "recruit": true,
        "orundum": true,
        "mining": true,
        "specialaccess": true
    }
    )");

    AsstAppendTask(ptr, "Roguelike", R"(
    {
        "theme": "Sarkaz",
        "mode": 1,
        "squad": "蓝图测绘分队",
        "roles": "稳扎稳打",
        "core_char": "维什戴尔"
    }
    )");*/

#else
    // AsstAppendTask(ptr, "Debug", nullptr);
#endif

    while (true) {
        AsstAppendTask(ptr, "Copilot", R"(
    {
        "enable": true,
        "filename": "D:\\MaaAssistantArknights\\x64\\RelWithDebInfo\\resource\\copilot\\OF-1_credit_fight.json"
    }
    )");
        AsstAsyncScreencap(ptr, true);
        AsstStart(ptr);

        while (AsstRunning(ptr)) {
            std::this_thread::yield();
        }
    }
    
    AsstStop(ptr);
    AsstDestroy(ptr);
    ptr = nullptr;

    return 0;
}
