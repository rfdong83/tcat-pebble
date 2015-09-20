/*
 * main.c
 * Sets up Window, AppMessage and a TextLayer to show the message received.
 */

#include <pebble.h>
#include <stdio.h>
  
#define KEY_DATA 5

static Window *s_main_window,*menu_window,*stop_window;
static TextLayer *s_output_layer,*stop_text_layer;
static SimpleMenuLayer *menu_layer;
static SimpleMenuSection sections[1];
static SimpleMenuItem items[5];
static char stops[5][30];
static char sched[5][100];


// APP MESSAGE---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


static void request_bus_data() {
  // Get time and weekday to give to phone
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  char weekday[18];
  char time[10];
  switch (tick_time->tm_wday) {
    case 0:
      strcpy(weekday,"Sunday");
      break;
    case 6: 
      strcpy(weekday,"Saturday");
    default:
      strcpy(weekday,"Monday - Friday");
  }
  clock_copy_time_string(time,sizeof(time));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Returned week day: %s", weekday);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Returned time: %s", time);
  // SEND IT
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_cstring(iter, 10, weekday);
  dict_write_cstring(iter, 11, time);
  app_message_outbox_send();
} 
  
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Get the first pair
  Tuple *t = dict_read_first(iterator);

  // Process all pairs present
  while (t != NULL) {

    // Process this pair's key
    switch (t->key) {
      case 0:
        // Copy value and display
        snprintf(stops[0], sizeof(stops[0]), "%s", t->value->cstring);
        break;
      case 1:
        // Copy value and display
        snprintf(stops[1], sizeof(stops[1]), "%s", t->value->cstring);
        break;
      case 2:
        // Copy value and display
        snprintf(stops[2], sizeof(stops[2]), "%s", t->value->cstring);
        break;
      case 3:
        // Copy value and display
        snprintf(stops[3], sizeof(stops[3]), "%s", t->value->cstring);
        break;
      case 4:
        // Copy value and display
        snprintf(stops[4], sizeof(stops[4]), "%s", t->value->cstring);
        break;
      case 5:
        // Copy value and display
        snprintf(sched[0], sizeof(sched[0]), "%s", t->value->cstring);
        break;
      case 6:
        // Copy value and display
        snprintf(sched[1], sizeof(sched[1]), "%s", t->value->cstring);
        break;
      case 7:
        // Copy value and display
        snprintf(sched[2], sizeof(sched[2]), "%s", t->value->cstring);
        break;
      case 8:
        // Copy value and display
        snprintf(sched[3], sizeof(sched[3]), "%s", t->value->cstring);
        break;
      case 9:
        // Copy value and display
        snprintf(sched[4], sizeof(sched[4]), "%s", t->value->cstring);
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

static void open_stop() {
  window_stack_push(stop_window, true);
}

static void menu_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  
  // Make SimpleItems
  int i = 0;
  for (; i < 5; i++) {
    items[i] = (SimpleMenuItem) {
      .title = stops[i],
      .callback = open_stop
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


// STOP WINDOW---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Stop screen load
static void stop_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  
  stop_text_layer = text_layer_create(window_bounds);
  text_layer_set_font(stop_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(stop_text_layer, "HALLEFUCKINLUJAH");
  text_layer_set_overflow_mode(stop_text_layer, GTextOverflowModeWordWrap);
  text_layer_set_text_alignment(stop_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(stop_text_layer));
}

// Stop screen unload
static void stop_window_unload(Window *window) {
  text_layer_destroy(stop_text_layer);
}

// MAIN WINDOW---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// Main screen click handler, opens next window
static void main_click_handler(ClickRecognizerRef recognizer, void *context) {
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
  text_layer_set_text(s_output_layer, "Welcome to the TCAT App!\n\nPress Any Key to Continue");
  text_layer_set_overflow_mode(s_output_layer, GTextOverflowModeWordWrap);
  text_layer_set_text_alignment(s_output_layer,GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer));
}

static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  text_layer_destroy(s_output_layer);
}


// INIT STUFF----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


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
  
  // Create menu Window
  menu_window = window_create();
  window_set_window_handlers(menu_window, (WindowHandlers) {
    .load = menu_window_load,
    .unload = menu_window_unload
  });
  
  // Create stop Window
  stop_window = window_create();
  window_set_window_handlers(stop_window, (WindowHandlers) {
    .load = stop_window_load,
    .unload = stop_window_unload
  });
  
  // Request Bus Data by giving time and day
  request_bus_data();
  
  // Start at main window
  window_stack_push(s_main_window, true);
}

static void deinit() {
  // Destroy main Window
  window_destroy(s_main_window);
  window_destroy(menu_window);
  window_destroy(stop_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
