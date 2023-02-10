// Copyright 2014 Wouter van Oortmerssen. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "lobster/stdafx.h"

#include "lobster/compiler.h"
#include "lobster/bytecode_generated.h"
#include "lobster/il.h"
#include "lobster/tonative.h"

using namespace lobster;

int main(int argc, char* argv[]) {
    #ifdef _MSC_VER
    	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        InitUnhandledExceptionFilter(argc, argv);
    #endif
    bool wait = false;
    bool from_bundle = false;
    #ifdef USE_EXCEPTION_HANDLING
    try
    #endif
    {
        bool dump_builtins = false;
        bool dump_names = false;
        bool compile_only = false;
        bool non_interactive_test = false;
        int runtime_checks = RUNTIME_ASSERT;
        string fn;
        vector<string> program_args;
        vector<string> imports;
        auto trace = TraceMode::OFF;
        string helptext = "\nUsage:\n"
            "lobster [ OPTIONS ] [ FILE ] [ -- ARGS ]\n"
            "Compile & run FILE, or omit FILE to load default.lpak\n"
            "--import RELDIR        Additional dir (relative to FILE) to load imports from\n"
            "--verbose              Output additional informational text.\n"
            "--debug                Output compiler internal logging.\n"
            "--silent               Only output errors.\n"
            "--runtime-shipping     Compile with asserts off.\n"
            "--runtime-asserts      Compile with asserts on (default).\n"
            "--runtime-verbose      Compile with asserts on + additional debug.\n"
            "--noconsole            Close console window (Windows).\n"
            "--gen-builtins-html    Write builtin commands help file.\n"
            "--gen-builtins-names   Write builtin commands - just names.\n"
            "--trace                Log bytecode instructions (SLOW, Debug only).\n"
            "--trace-tail           Show last 50 bytecode instructions on error.\n"
            "--wait                 Wait for input before exiting.\n";
            int arg = 1;
        for (; arg < argc; arg++) {
            if (argv[arg][0] == '-') {
                string a = argv[arg];
                if      (a == "--wait") { wait = true; }
                else if (a == "--verbose") { min_output_level = OUTPUT_INFO; }
                else if (a == "--debug") { min_output_level = OUTPUT_DEBUG; }
                else if (a == "--silent") { min_output_level = OUTPUT_ERROR; }
                else if (a == "--runtime-shipping") { runtime_checks = RUNTIME_NO_ASSERT; }
                else if (a == "--runtime-asserts") { runtime_checks = RUNTIME_ASSERT; }
                else if (a == "--runtime-verbose") { runtime_checks = RUNTIME_ASSERT_PLUS; }
                else if (a == "--runtime-debug") { runtime_checks = RUNTIME_DEBUG; }
                else if (a == "--noconsole") { SetConsole(false); }
                else if (a == "--gen-builtins-html") { dump_builtins = true; }
                else if (a == "--gen-builtins-names") { dump_names = true; }
                else if (a == "--compile-only") { compile_only = true; }
                else if (a == "--trace") { trace = TraceMode::ON; runtime_checks = RUNTIME_DEBUG; }
                else if (a == "--trace-tail") { trace = TraceMode::TAIL; runtime_checks = RUNTIME_DEBUG; }
                else if (a == "--import") {
                    arg++;
                    if (arg >= argc) THROW_OR_ABORT("missing import dir");
                    imports.push_back(argv[arg]);
                } else if (a == "--") {
                    arg++;
                    break;
                }
                else THROW_OR_ABORT("unknown command line argument: " + (argv[arg] + helptext));
            } else {
                if (!fn.empty()) THROW_OR_ABORT("more than one file specified" + helptext);
                fn = SanitizePath(argv[arg]);
            }
        }
        if (fn.empty())
            THROW_OR_ABORT("no file name given");
        for (; arg < argc; arg++) { program_args.push_back(argv[arg]); }

        NativeRegistry nfr;
        RegisterCoreLanguageBuiltins(nfr);
        auto loader = EnginePreInit(nfr);

        if (!InitPlatform(GetMainDirFromExePath(argv[0]), fn, false, loader))
            THROW_OR_ABORT("cannot find location to read/write data on this platform!");

        if (dump_builtins) { DumpBuiltinDoc(nfr); return 0; }
        if (dump_names) { DumpBuiltinNames(nfr); return 0; }

        for (auto &import : imports) AddDataDir(import, true);
        fn = StripDirPart(fn);

        string bytecode_buffer;
        Compile(nfr, fn, string_view(), bytecode_buffer, nullptr, nullptr, false, runtime_checks);
        string error;
        auto ret = RunTCC(nfr, bytecode_buffer, fn, nullptr, std::move(program_args),
                          TraceMode::OFF, false, error, runtime_checks, false);
        if (!error.empty())
            THROW_OR_ABORT(error);
        // LOG_INFO(ret.first); LOG_INFO(ret.second);
        return (int)ret.second;
    }
    #ifdef USE_EXCEPTION_HANDLING
    catch (string &s) {
        LOG_ERROR(s);
        if (wait) {
            LOG_PROGRAM("press <ENTER> to continue:\n");
            getchar();
        }
        #ifdef _MSC_VER
            _CrtSetDbgFlag(0);  // Don't bother with memory leaks when there was an error.
        #endif
        return 1;
    }
    #endif
    return 0;
}
