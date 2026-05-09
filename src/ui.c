#include "sokol_app.h"
#define CLAY_IMPLEMENTATION
#include "draw.h"
#include "ui.h"

#include <stdlib.h>
#include <stdio.h>

#include "base.h"

static struct {
    Clay_Arena clay_memory;
} ui;

static void ui_handle_clay_errors(Clay_ErrorData errorData);
static void ui_clay_init(void) {
    uint64_t total_mem_size = Clay_MinMemorySize();
    ui.clay_memory = Clay_CreateArenaWithCapacityAndMemory(
        total_mem_size,
        malloc(total_mem_size)
    );
    Clay_Initialize(
        ui.clay_memory,
        (Clay_Dimensions) {
            (float)sapp_widthf()/sapp_dpi_scale(),
            (float)sapp_heightf()/sapp_dpi_scale()
        },
        (Clay_ErrorHandler) { ui_handle_clay_errors, 0 }
    );
}

static void ui_clay_free(void) {
    free(ui.clay_memory.memory);
}

static void ui_handle_clay_errors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);

    switch (errorData.errorType) {
        case CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED: {
            Clay_SetMaxElementCount(Clay_GetMaxElementCount() * 2);
            ui_clay_free();
            ui_clay_init();
        } break;

        case CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED: {
            Clay_SetMaxMeasureTextCacheWordCount(
                Clay_GetMaxMeasureTextCacheWordCount() * 2
            );
            ui_clay_free();
            ui_clay_init();
        } break;

        default:
            break;
    }
}

static Clay_Dimensions ui_measure_text(
    Clay_StringSlice text,
    Clay_TextElementConfig *config,
    void *_
) {
    draw_TextSize size = draw_measure_str((char *)text.chars, text.length, config->fontSize);
    return (Clay_Dimensions) {
        .width = size.width,
        .height = size.height,
    };
}

void ui_init(void) {
    ui_clay_init();
    Clay_SetMeasureTextFunction(ui_measure_text, NULL);
}

void ui_free(void) {
    ui_clay_free();
}

void ui_update(void) {
}

void ui_render(Clay_RenderCommandArray render_cmds, draw_Geo *geo) {
    for (int j = 0; j < render_cmds.length; j++) {
        Clay_RenderCommand *cmd = Clay_RenderCommandArray_Get(
            &render_cmds,
            j
        );
        Clay_BoundingBox bbox = {
            cmd->boundingBox.x,
            cmd->boundingBox.y,
            cmd->boundingBox.width,
            cmd->boundingBox.height
        };
        switch (cmd->commandType) {
            case CLAY_RENDER_COMMAND_TYPE_TEXT: {
                Clay_TextRenderData *trd = &cmd->renderData.text;

                draw_geo_line(
                    geo,
                    (f2) { bbox.x, bbox.y },
                    (f2) { bbox.x + bbox.width, bbox.y + bbox.height },
                    10.0f,
                    (Color) { 255, 0, 0, 255 }
                );
                draw_geo_line(
                    geo,
                    (f2) { bbox.x, bbox.y + bbox.height },
                    (f2) { bbox.x + bbox.width, bbox.y },
                    10.0f,
                    (Color) { 255, 0, 0, 255 }
                );

                draw_geo_str_ui(
                    geo,
                    (f2){ bbox.x, bbox.y },
                    (char *)trd->stringContents.chars,
                    trd->stringContents.length,
                    trd->fontSize,
                    (Color) {
                        trd->textColor.r,
                        trd->textColor.g,
                        trd->textColor.b,
                        trd->textColor.a,
                    }
                );

                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
            }
            case CLAY_RENDER_COMMAND_TYPE_OVERLAY_COLOR_START: {
            }
            case CLAY_RENDER_COMMAND_TYPE_OVERLAY_COLOR_END: {
            }
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
            }
            case CLAY_RENDER_COMMAND_TYPE_BORDER: {
            }
            case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
            }
            default: {
                printf("Error: unhandled render command.");
                exit(1);
            }
        }
    }
}
