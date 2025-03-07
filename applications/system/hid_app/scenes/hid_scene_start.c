#include "../hid.h"
#include "../views.h"

enum HidSubmenuIndex {
    HidSubmenuIndexKeynote,
    HidSubmenuIndexKeynoteVertical,
    HidSubmenuIndexKeyboard,
    HidSubmenuIndexMedia,
    HidSubmenuIndexTikTok,
    HidSubmenuIndexMouse,
    HidSubmenuIndexMouseClicker,
    HidSubmenuIndexMouseJiggler,
    HidSubmenuIndexRemovePairing,
};

static void hid_scene_start_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    Hid* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void hid_scene_start_on_enter(void* context) {
    Hid* app = context;

    // We're bypassing all other submenu additions and focusing on Media
    submenu_add_item(
        app->submenu, "Media", HidSubmenuIndexMedia, hid_scene_start_submenu_callback, app);

    // Directly set the selected item to Media without waiting for user input
    submenu_set_selected_item(app->submenu, HidSubmenuIndexMedia);
    view_dispatcher_switch_to_view(app->view_dispatcher, HidViewMedia);
}

bool hid_scene_start_on_event(void* context, SceneManagerEvent event) {
    Hid* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == HidSubmenuIndexRemovePairing) {
            scene_manager_next_scene(app->scene_manager, HidSceneUnpair);
        } else {
            HidView view_id;

            switch(event.event) {
            case HidSubmenuIndexKeynote:
                view_id = HidViewKeynote;
                hid_keynote_set_orientation(app->hid_keynote, false);
                break;
            case HidSubmenuIndexKeynoteVertical:
                view_id = HidViewKeynote;
                hid_keynote_set_orientation(app->hid_keynote, true);
                break;
            case HidSubmenuIndexKeyboard:
                view_id = HidViewKeyboard;
                break;
            case HidSubmenuIndexMedia:
                view_id = HidViewMedia;
                break;
            case HidSubmenuIndexTikTok:
                view_id = BtHidViewTikTok;
                break;
            case HidSubmenuIndexMouse:
                view_id = HidViewMouse;
                break;
            case HidSubmenuIndexMouseClicker:
                view_id = HidViewMouseClicker;
                break;
            case HidSubmenuIndexMouseJiggler:
                view_id = HidViewMouseJiggler;
                break;
            default:
                furi_crash();
            }

            scene_manager_set_scene_state(app->scene_manager, HidSceneMain, view_id);
            scene_manager_next_scene(app->scene_manager, HidSceneMain);
        }

        scene_manager_set_scene_state(app->scene_manager, HidSceneStart, event.event);
        consumed = true;
    }

    return consumed;
}

void hid_scene_start_on_exit(void* context) {
    Hid* app = context;
    submenu_reset(app->submenu);
}
