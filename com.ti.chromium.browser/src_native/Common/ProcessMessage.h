#ifndef _PROCESS_MESSAGE_H_
#define _PROCESS_MESSAGE_H_
#pragma once

#define JSON_ROOT "result"

/* Expression evaluation constants */
#define MSG_EVAL "eval"
#define MSG_EVAL_INDEX 0

#define MSG_EVAL_RESPOND "eval_respond"
#define MSG_EVAL_RESPOND_RESULT_INDEX 0
#define MSG_EVAL_RESPOND_EXCEPTION_INDEX 1

#if defined(WIN32)
#define IPC_V8_PIPE_NAME "\\\\.\\pipe\\_CEF3_V8_PIPE_"
#define IPC_NAVIGATION_PIPE_NAME "\\\\.\\pipe\\_CEF3_NAVIGATION_PIPE_"
#elif defined(linux)
#define IPC_V8_PIPE_NAME "/tmp/_CEF3_V8_PIPE_"
#define IPC_NAVIGATION_PIPE_NAME "/tmp/_CEF3_NAVIGATION_PIPE_"
#endif

#define MSG_RENDER_CONTEXT_CREATED "context_created"

#define MSG_ADD_FUNCTION "add_function"
#define MSG_ADD_FUNCTION_NAME_INDEX 0
#define MSG_REMOVE_FUNCTION "remove_function"
#define MSG_REMOVE_FUNCTION_NAME_INDEX 0

#define MSG_FUNCTION_CALLBACK "function_callback"
#define MSG_FUNCTION_CALLBACK_FUNC_NAME_INDEX 1
#define MSG_FUNCTION_CALLBACK_FUNC_ARGS_INDEX 2

#define MSG_BEFORE_NAVIGATE "before_navigate"
#define MSG_BEFORE_NAVIGATE_INDEX 1

#define CEF_SUCCEEDED "SUCCEEDED"
#define CEF_FAILED "FAILED"

#endif
