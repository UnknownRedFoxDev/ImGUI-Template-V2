#define NOB_IMPLEMENTATION
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "thirdparty/nob.h"

#include <stdbool.h>

#define CXX "g++"
#define SRC_DIR "src/"
#define BUILD_DIR "build/"
#define BIN_DIR BUILD_DIR "bin/"
#define THIRDPARTY_DIR "thirdparty/"
#define IMGUI_DIR THIRDPARTY_DIR "imgui_1.92.7/"
#define LIBPATH BUILD_DIR "modules.a"

#define da_get(da, idx) (da)->items[i]

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

bool DEBUG = false;

typedef struct {
    const char *path;
    const char *name;
} submodule;

typedef struct {
    submodule *items;
    size_t count;
    size_t capacity;
} submodules;

void compile_command(Nob_Cmd *cmd, const char *inputPath, const char *outputPath, bool linking)
{
    cmd_append(cmd, CXX);
    cmd_append(cmd, "-std=c++23");
    cmd_append(cmd, "-Wall");
    cmd_append(cmd, "-Wextra");
    cmd_append(cmd, "-Wformat");
    if (DEBUG) cmd_append(cmd, "-g");
    if (DEBUG) cmd_append(cmd, "-ggdb");
    cmd_append(cmd, "-o", outputPath);
    if (!linking) cmd_append(cmd, "-c", inputPath);
    else {
        cmd_append(cmd, inputPath);
        cmd_append(cmd, LIBPATH);
    }

    cmd_append(cmd, temp_sprintf("-I%s", SRC_DIR));
    cmd_append(cmd, temp_sprintf("-I%s", IMGUI_DIR));
    cmd_append(cmd, temp_sprintf("-I%s/backends", IMGUI_DIR));
    cmd_append(cmd, "-lGL");
    cmd_append(cmd, "-ldl");
    cmd_append(cmd, "-lSDL3");
}

bool compile_submodules(submodules *modules, bool *needsRecompile)
{
    Cmd cmd = {0};
    Procs procs = {0};
    bool result = true;

    da_foreach (submodule, module, modules) {
        size_t mark = nob_temp_save();
        char *input = nob_temp_sprintf("%s%s.cpp", module->path, module->name);
        char *output = nob_temp_sprintf("%s%s.o", BUILD_DIR, module->name);
        if (nob_needs_rebuild1(output, input) || DEBUG) {
            *needsRecompile = true;
            printf("-----------------------------------\n");
            nob_log(INFO, "%sCompiling:%s %s", COLOR_BLUE, COLOR_RESET, input);
            compile_command(&cmd, input, output, false); // No linking yet
            if (!cmd_run(&cmd, .async = &procs)) {
                nob_log(ERROR, "%sFailed%s to compile %s", COLOR_RED, COLOR_RESET, module->name);
                printf("-----------------------------------\n");
                return_defer(false);
            }
            nob_log(INFO, "%sSuccessfully%s compiled %s", COLOR_GREEN, COLOR_RESET, module->name);
            printf("-----------------------------------\n");
        }
        nob_temp_rewind(mark);
    }

    if (!procs_flush(&procs)) return_defer(false);

    if (*needsRecompile) {
        if (file_exists(LIBPATH)) delete_file(LIBPATH);

        cmd_append(&cmd, "ar", "rcs");
        cmd_append(&cmd, LIBPATH);
        da_foreach(submodule, it, modules) {
            cmd_append(&cmd, temp_sprintf("%s%s.o", BUILD_DIR, it->name));
        }
        if (!nob_cmd_run(&cmd)) return_defer(1);
    }

defer:
    free(cmd.items);
    free(procs.items);
    return result;
}

bool compile_main(Nob_Cmd *cmd, bool needsRecompile)
{
    if (needsRecompile && file_exists(BIN_DIR "main"))
        delete_file(BIN_DIR "main");

    if (nob_needs_rebuild1(BIN_DIR "main", SRC_DIR "main.cpp") || DEBUG) {
        compile_command(cmd, SRC_DIR "main.cpp", BIN_DIR "main", true);
        if (!nob_cmd_run(cmd)) return false;
    }

    return true;
}

bool compile_imgui(bool *needsRecompile)
{
    submodules imguiModules = {0};
    bool result = true;

    da_append(&imguiModules, ((submodule){IMGUI_DIR, "imgui"}));
    da_append(&imguiModules, ((submodule){IMGUI_DIR, "imgui_demo"}));
    da_append(&imguiModules, ((submodule){IMGUI_DIR, "imgui_draw"}));
    da_append(&imguiModules, ((submodule){IMGUI_DIR, "imgui_tables"}));
    da_append(&imguiModules, ((submodule){IMGUI_DIR, "imgui_widgets"}));
    da_append(&imguiModules, ((submodule){IMGUI_DIR "backends/", "imgui_impl_sdl3"}));
    da_append(&imguiModules, ((submodule){IMGUI_DIR "backends/", "imgui_impl_opengl3"}));
    if (!compile_submodules(&imguiModules, needsRecompile)) result = false;
    free(imguiModules.items);
    return result;
}

bool initialise_directories(bool clean)
{
    Nob_Cmd cmd = {0};
    bool result = true;

    if (clean) {
        if (file_exists(BUILD_DIR)) {
            cmd_append(&cmd, "rm", "-rf", BUILD_DIR);
            if (!nob_cmd_run(&cmd)) return_defer(false);
        }
    }
    if (!clean) minimal_log_level = WARNING;
    if (!nob_mkdir_if_not_exists(BUILD_DIR)) return_defer(false);
    if (!nob_mkdir_if_not_exists(BIN_DIR)) return_defer(false);
    if (!clean) minimal_log_level = INFO;

defer:
    free(cmd.items);
    return result;
}

int main(int argc, char **argv)
{
    GO_REBUILD_URSELF(argc, argv);

    int result = 0;
    bool needsRecompile = false;

    Nob_Cmd cmd = {0};
    // bool *help;
    bool clean = false;
    // bool *run;

    if (!initialise_directories(clean)) return_defer(1);
    if (!compile_imgui(&needsRecompile)) return_defer(1);
    if (!compile_main(&cmd, needsRecompile)) return_defer(1);

defer:
    free(cmd.items);
    return result;
}
