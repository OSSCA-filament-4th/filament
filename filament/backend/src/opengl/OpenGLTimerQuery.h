/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TNT_FILAMENT_BACKEND_OPENGL_TIMERQUERY_H
#define TNT_FILAMENT_BACKEND_OPENGL_TIMERQUERY_H

#include "OpenGLDriver.h"

#include <utils/Condition.h>

#include <thread>
#include <vector>

namespace filament::backend {

/*
 * we need two implementation of timer queries (only elapsed time), because
 * on some gpu disjoint_timer_query/arb_timer_query is much less accurate than
 * using fences.
 *
 * These classes implement the various strategies...
 */

class OpenGLTimerQueryInterface {
protected:
    using GLTimerQuery = OpenGLDriver::GLTimerQuery;
    using clock = std::chrono::steady_clock;

public:
    virtual ~OpenGLTimerQueryInterface();
    virtual void beginTimeElapsedQuery(GLTimerQuery* query) = 0;
    virtual void endTimeElapsedQuery(GLTimerQuery* query) = 0;
    virtual bool queryResultAvailable(GLTimerQuery* query) = 0;
    virtual uint64_t queryResult(GLTimerQuery* query) = 0;
};

#if defined(BACKEND_OPENGL_VERSION_GL) || defined(GL_EXT_disjoint_timer_query)

class TimerQueryNative : public OpenGLTimerQueryInterface {
public:
    explicit TimerQueryNative(OpenGLContext& context);
    ~TimerQueryNative() override;
private:
    void beginTimeElapsedQuery(GLTimerQuery* query) override;
    void endTimeElapsedQuery(GLTimerQuery* query) override;
    bool queryResultAvailable(GLTimerQuery* query) override;
    uint64_t queryResult(GLTimerQuery* query) override;
    OpenGLContext& mContext;
};

#endif

class OpenGLTimerQueryFence : public OpenGLTimerQueryInterface {
public:
    explicit OpenGLTimerQueryFence(OpenGLPlatform& platform);
    ~OpenGLTimerQueryFence() override;
private:
    using Job = std::function<void()>;
    void beginTimeElapsedQuery(GLTimerQuery* query) override;
    void endTimeElapsedQuery(GLTimerQuery* query) override;
    bool queryResultAvailable(GLTimerQuery* query) override;
    uint64_t queryResult(GLTimerQuery* query) override;
    void enqueue(Job&& job);

    template<typename CALLABLE, typename ... ARGS>
    void push(CALLABLE&& func, ARGS&& ... args) {
        enqueue(Job(std::bind(std::forward<CALLABLE>(func), std::forward<ARGS>(args)...)));
    }

    OpenGLPlatform& mPlatform;
    std::thread mThread;
    mutable utils::Mutex mLock;
    mutable utils::Condition mCondition;
    std::vector<Job> mQueue;
    bool mExitRequested = false;
};

class TimerQueryFallback : public OpenGLTimerQueryInterface {
public:
    explicit TimerQueryFallback();
    ~TimerQueryFallback() override;
private:
    void beginTimeElapsedQuery(GLTimerQuery* query) override;
    void endTimeElapsedQuery(GLTimerQuery* query) override;
    bool queryResultAvailable(GLTimerQuery* query) override;
    uint64_t queryResult(GLTimerQuery* query) override;
};

} // namespace filament::backend

#endif //TNT_FILAMENT_BACKEND_OPENGL_TIMERQUERY_H
