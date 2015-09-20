/*
 * main.c
 * Sets up Window, AppMessage and a TextLayer to show the message received.
 */

#include <pebble.h>
#include <stdio.h>
  
#define KEY_DATA 5

static Window *s_main_window,*menu_window,*stop_window;
static TextLayer *s_output_layer;
static SimpleMenuLayer *menu_layer, *stop_layer;
static SimpleMenuSection sections[1];
static SimpleMenuItem items[5];
static char stops[5][30];
static char sched[5][100];
static char stop_text[100];

char *
strtok(s, delim)
	register char *s;
	register const char *delim;
{
	register char *spanp;
	register int c, sc;
	char *tok;
	static char *last;


	if (s == NULL && (s = last) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		last = NULL;
		return (NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}


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
      case 12:
        // Request Bus Data by giving time and day
        request_bus_data();
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

static void open_stop(int i, void* context) {
  window_stack_push(stop_window, true);
  strcpy(stop_text, sched[i]);
  APP_LOG(APP_LOG_LEVEL_DEBUG,stop_text);
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

static void stop_click_handler(ClickRecognizerRef recognizer, void *context) {
  window_stack_remove(stop_window,true);
}

// Main click config, same for all buttons
static void stop_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_BACK, stop_click_handler);
}


// Stop screen load
static void stop_window_load(Window *window) {
  Layer *stop_window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(stop_window_layer);
  
  int empty = 0;
  
  // Make SimpleItems
  int i = 0;
  for (; i < 5; i++) {
    char* title = strtok(stop_text, ",");
    char* sub = strtok(stop_text, ",");
    strtok(stop_text,",");
    if (!title) {
      empty = 1;
      break;
    }
    items[i] = (SimpleMenuItem) {
      .title = title,
      .subtitle = sub
    };
  }
  
  // Populating the section with items
  if (!empty) {
    sections[0] = (SimpleMenuSection) {
    .title = "Incoming Buses",
    .num_items = 5,
    .items = items
    };
  }
  else {
    items[0] = (SimpleMenuItem) {
      .title = "No buses available at the moment"
    };
    sections[0] = (SimpleMenuSection) {
      .title = "Incoming Buses",
      .num_items = 1,
      .items = items
    };
  }

  
  // Create SimpleMenuLayer
  stop_layer = simple_menu_layer_create(window_bounds,window,sections,1,NULL);
  layer_add_child(stop_window_layer,simple_menu_layer_get_layer(stop_layer));
}

// Stop screen unload
static void stop_window_unload(Window *window) {
  simple_menu_layer_destroy(stop_layer);
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
  window_set_click_config_provider(stop_window, stop_click_config_provider);
    
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
