/*
** Copyright 2018 Bloomberg Finance L.P.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#ifndef QUANTUM_DISPATCHER_CORE_H
#define QUANTUM_DISPATCHER_CORE_H

#include <vector>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <thread>
#include <functional>
#include <algorithm>
#include <pthread.h>
#include <quantum/quantum_task_queue.h>
#include <quantum/quantum_io_queue.h>

namespace Bloomberg {
namespace quantum {

//==============================================================================================
//                                 class DispatcherCore
//==============================================================================================
/// @class DispatcherCore
/// @brief Class implementing the dispatching logic unto worker threads. Used for both coroutines
///        and IO tasks.
/// @note For internal use only.
class DispatcherCore : public ITerminate
{
public:
    friend class TaskDispatcher;
    
    ~DispatcherCore();
    
    void terminate() final;
    
    size_t size(IQueue::QueueType type, int queueId) const;
    
    bool empty(IQueue::QueueType type, int queueId) const;
    
    QueueStatistics stats(IQueue::QueueType type, int queueId);
    
    void resetStats();
    
    void post(Task::ptr task);
    
    void postAsyncIo(IoTask::ptr task);

private:
    DispatcherCore(int numCoroutineThreads,
                   int numIoThreads,
                   bool pinCoroutineThreadsToCores);
    
    size_t coroSize(int queueId) const;
    
    size_t ioSize(int queueId) const;
    
    bool coroEmpty(int queueId) const;
    
    bool ioEmpty(int queueId) const;
    
    QueueStatistics coroStats(int queueId);
    
    QueueStatistics ioStats(int queueId);
    
    //Members
    std::vector<TaskQueue>  _coroQueues;    //coroutine queues
    IoQueue                 _sharedIoQueue; //shared IO task queue (holds tasks posted to 'Any' IO queue)
    std::vector<IoQueue>    _ioQueues;      //dedicated IO task queues
    std::atomic_flag        _terminated;
};

}}

#include <quantum/impl/quantum_dispatcher_core_impl.h>

#endif //QUANTUM_DISPATCHER_CORE_H
