#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>

void vibro_test_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Vibro application");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 22, "Vibrating... Release DOWN to stop");
}

void vibro_test_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t vibro_test_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, vibro_test_draw_callback, NULL);
    view_port_input_callback_set(view_port, vibro_test_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);

    // Assume DOWN button is already pressed at the start
    notification_message(notification, &sequence_set_vibro_on);
    notification_message(notification, &sequence_set_green_255);

    InputEvent event;

    while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk) {
        if(event.type == InputTypeShort && event.key == InputKeyBack) {
            // Stop vibration on Back button
            notification_message(notification, &sequence_reset_vibro);
            notification_message(notification, &sequence_reset_green);
            break;
        }
        if(event.key == InputKeyDown && event.type == InputTypeRelease) {
            // Stop vibration and exit on DOWN button release
            notification_message(notification, &sequence_reset_vibro);
            notification_message(notification, &sequence_reset_green);
            break;
        }
    }

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);

    return 0;
}

