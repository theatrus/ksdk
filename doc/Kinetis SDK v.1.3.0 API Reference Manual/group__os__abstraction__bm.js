var group__os__abstraction__bm =
[
    [ "semaphore_t", "group__os__abstraction__bm.html#structsemaphore__t", [
      [ "isWaiting", "group__os__abstraction__bm.html#ae48a6200f3625d1c3a6efe8cd0d2f58c", null ],
      [ "semCount", "group__os__abstraction__bm.html#ab09113d6c45205405bee97dd21d678d1", null ],
      [ "time_start", "group__os__abstraction__bm.html#ac766dca95b95cc91ff2e1de38aeb60f1", null ],
      [ "timeout", "group__os__abstraction__bm.html#ada1d0273ebf4106140af0f4b69dc4cb1", null ]
    ] ],
    [ "mutex_t", "group__os__abstraction__bm.html#structmutex__t", [
      [ "isWaiting", "group__os__abstraction__bm.html#a8aabb2adca63fecf51370c3fb53262b6", null ],
      [ "isLocked", "group__os__abstraction__bm.html#a7fdb392ab9f49cb9c065218de82897a2", null ],
      [ "time_start", "group__os__abstraction__bm.html#aaacd05ad16d658a841ea0483aa43f229", null ],
      [ "timeout", "group__os__abstraction__bm.html#a780316dbcf772f6dd075b89e6486a437", null ],
      [ "sema", "group__os__abstraction__bm.html#ac979df92cafe41f627f2568cb7c89a59", null ],
      [ "owner", "group__os__abstraction__bm.html#a778cf4e788153c24b1c126157d4ab4b3", null ]
    ] ],
    [ "event_t", "group__os__abstraction__bm.html#structevent__t", [
      [ "isWaiting", "group__os__abstraction__bm.html#a008c904a9e85e0c4d0e276554807883d", null ],
      [ "time_start", "group__os__abstraction__bm.html#a3a2b3f76c7a312407c24d67045a70c25", null ],
      [ "timeout", "group__os__abstraction__bm.html#a5834d5bd66eefe0625fa90cbc095d2b9", null ],
      [ "flags", "group__os__abstraction__bm.html#af1d71e2dffc5ca2c382f7ff2e5c8f187", null ],
      [ "clearMode", "group__os__abstraction__bm.html#adbc60a9072236c3f86e1b1644406a4c6", null ]
    ] ],
    [ "task_control_block_t", "group__os__abstraction__bm.html#structtask__control__block__t", [
      [ "p_func", "group__os__abstraction__bm.html#a769818bc11ec679785a6a0c8b8a22018", null ],
      [ "param", "group__os__abstraction__bm.html#a1d27a3a4aa8ff46d82d6265a4266ab36", null ],
      [ "next", "group__os__abstraction__bm.html#a12a1b9ba901cfdc48b4031bab1936aad", null ],
      [ "prev", "group__os__abstraction__bm.html#a6a0029799e0fcf74baaa1bc9be165df3", null ]
    ] ],
    [ "msg_queue_t", "group__os__abstraction__bm.html#structmsg__queue__t", [
      [ "queueMem", "group__os__abstraction__bm.html#a1e1376fb579cd7452ec82b74e20becc0", null ],
      [ "number", "group__os__abstraction__bm.html#ad9821e6a3f798ee9f4e3f952c2a214ac", null ],
      [ "size", "group__os__abstraction__bm.html#aa911786bd45fa3e09ccdaf4d9ac56228", null ],
      [ "head", "group__os__abstraction__bm.html#ac52c783c93cd4f8137b84be35e211dde", null ],
      [ "tail", "group__os__abstraction__bm.html#a50e20ae9630c8753c5d28082342f4bab", null ],
      [ "queueSem", "group__os__abstraction__bm.html#a079cc807babeda1b0aa1a3b140093be7", null ],
      [ "isEmpty", "group__os__abstraction__bm.html#a2e7ee3f65b00f6e92129abe757286323", null ]
    ] ],
    [ "Bare Metal Abstraction Layer", "group__os__abstraction__bm.html#osa_bm", null ],
    [ "FSL_OSA_BM_TIMER_NONE", "group__os__abstraction__bm.html#ga5ffcf301e911e4911ebb33f6d120b79b", null ],
    [ "FSL_OSA_BM_TIMER_LPTMER", "group__os__abstraction__bm.html#gadd1345edb8713252108dffd4f7d961ab", null ],
    [ "FSL_OSA_BM_TIMER_CONFIG", "group__os__abstraction__bm.html#gad95549b5ccdee2644886381c47507d1d", null ],
    [ "OSA_WAIT_FOREVER", "group__os__abstraction__bm.html#ga5c4bbd97efaa670e198fad62038ba771", null ],
    [ "TASK_MAX_NUM", "group__os__abstraction__bm.html#ga11a0fb3f79cc6bab55d41a1f2e1789da", null ],
    [ "FSL_OSA_TIME_RANGE", "group__os__abstraction__bm.html#ga0b8cb2e5f9f9863520c4c9e4c3c47e3f", null ],
    [ "OSA_DEFAULT_INT_HANDLER", "group__os__abstraction__bm.html#ga50f035ff4993767f49ca67b3847bfec4", null ],
    [ "OSA_TASK_DEFINE", "group__os__abstraction__bm.html#ga37ea523ee533242cf8b8c3706a93d818", null ],
    [ "MSG_QUEUE_DECLARE", "group__os__abstraction__bm.html#gad27d9381348700acda99a4eedad3a0d7", null ],
    [ "event_flags_t", "group__os__abstraction__bm.html#ga79a78f74479544e6d60e4ecaedf83752", null ],
    [ "task_param_t", "group__os__abstraction__bm.html#ga90774a4d1708f3e210d94c829b44eb86", null ],
    [ "task_t", "group__os__abstraction__bm.html#gad138ff72ead35c2ed90c90302fb0da1e", null ],
    [ "task_handler_t", "group__os__abstraction__bm.html#ga2d867ba7d27cb355a1827c2c11889bcc", null ],
    [ "task_stack_t", "group__os__abstraction__bm.html#ga184cb36aad264b6917c489bbc2b793a5", null ],
    [ "msg_queue_handler_t", "group__os__abstraction__bm.html#ga04ee0d756e55f89abf869dcab383535c", null ],
    [ "DefaultISR", "group__os__abstraction__bm.html#ga4fcbeb17b7f10e57e297bf6b12a07e7e", null ],
    [ "OSA_PollAllOtherTasks", "group__os__abstraction__bm.html#ga5b58984db92662727d4c6e612029ebb4", null ]
];