/*
 * main.c
 * Sets up Window, AppMessage and a TextLayer to show the message received.
 */

#include <pebble.h>

#define KEY_DATA 5

static Window *s_main_window,*menu_window;
static TextLayer *s_output_layer;
static SimpleMenuLayer *menu_layer;
static SimpleMenuSection sections[1];
static SimpleMenuItem items[5];


// APP MESSAGE---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Get the first pair
  Tuple *t = dict_read_first(iterator);

  // Process all pairs present
  while (t != NULL) {
    // Long lived buffer
    static char s_buffer[64];

    // Process this pair's key
    switch (t->key) {
      case KEY_DATA:
        // Copy value and display
        snprintf(s_buffer, sizeof(s_buffer), "Received '%s'", t->value->cstring);
//         text_layer_set_text(s_output_layer, s_buffer);
        break;
    }

    // Get next pair, if any
    t = dict_read_next(iterator);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


// MENU WINDOW---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void test() {
  
}

static void menu_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  
  // Make SimpleItems
  int i = 0;
  for (; i < 5; i++) {
    items[i] = (SimpleMenuItem) {
      .title = "Item",
      .callback = test
    };
  }
  
  // Populating the section with items
  sections[0] = (SimpleMenuSection) {
    .title = "The Five Closest Stops",
    .num_items = 5,
    .items = items
  };
  
  // Create SimpleMenuLayer
  menu_layer = simple_menu_layer_create(window_bounds,window,sections,1,NULL);
  layer_add_child(window_layer,simple_menu_layer_get_layer(menu_layer));
}

static void menu_window_unload(Window *window) {
  simple_menu_layer_destroy(menu_layer);
}

// MAIN WINDOW---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// Main screen click handler, opens next window
static void main_click_handler(ClickRecognizerRef recognizer, void *context) {
  menu_window = window_create();
  window_set_window_handlers(menu_window, (WindowHandlers) {
    .load = menu_window_load,
    .unload = menu_window_unload
  });
  window_stack_push(menu_window,true);
}

// Main click config, same for all buttons
static void main_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, main_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, main_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, main_click_handler);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Create output TextLayer
  s_output_layer = text_layer_create(GRect(5, 0, window_bounds.size.w - 5, window_bounds.size.h));
  text_layer_set_font(s_output_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(s_output_layer, "Welcome to the TCAT App...");
  text_layer_set_overflow_mode(s_output_layer, GTextOverflowModeWordWrap);
  text_layer_set_text_alignment(s_output_layer,GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer));
}

static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  text_layer_destroy(s_output_layer);
}

static void init() {
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_click_config_provider(s_main_window, main_click_config_provider);
  window_stack_push(s_main_window, true);
}

static void deinit() {
  // Destroy main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
