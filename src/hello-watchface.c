#include <pebble.h>

static Window *window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static Layer *s_weather_status_layer;
static TextLayer *s_weather_status_text;
static TextLayer *s_temperature_layer;
static TextLayer *s_conditions_layer;
static TextLayer *s_conditions_id_layer;
static TextLayer *s_lat_text;
static TextLayer *s_long_text;
static GFont s_time_font;
static GFont s_temperature_font;

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_CONDITIONS_ID 2
#define KEY_LOG 3
#define KEY_PING 4
#define KEY_LAT 5
#define KEY_LONG 6

static void click_config_provider(void *context) {

}


static void send_weather_update_request () {
  DictionaryIterator *iter;

  app_message_outbox_begin(&iter);
  Tuplet value = TupletInteger(0, KEY_PING);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
}

static void update_status(char *message) {
  static char status_buffer[32];

  snprintf(status_buffer, sizeof(status_buffer), "%s", message);
  text_layer_set_text(s_weather_status_text, status_buffer);
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char time_buffer[] = "00:00";
  static char date_buffer[] = "2015-12-31";
  int index, length;

  if (clock_is_24h_style() == true) {
    strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    if (tick_time->tm_hour % 12 < 10 &&
        tick_time->tm_hour % 12 != 0) {
      strftime(time_buffer, sizeof("00:00"), "%l:%M", tick_time);
      length = sizeof("00:00");
      for (index = 0; index < length - 1; index++) {
        time_buffer[index] = time_buffer[index + 1];
      }

    } else {
      strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
    }
  }

  if (tick_time->tm_min % 30 == 0) {
    send_weather_update_request();
  }

  strftime(date_buffer, sizeof("2015-12-31"), "%Y-%m-%d", tick_time);

  text_layer_set_text(s_time_layer, time_buffer);
  text_layer_set_text(s_date_layer, date_buffer);
}

static void update_lat(char*message) {
  static char lat_buffer[16];

  snprintf(lat_buffer, sizeof(lat_buffer), "%s", message);
  text_layer_set_text(s_lat_text, lat_buffer);
}

static void update_long(char*message) {
  static char long_buffer[16];

  snprintf(long_buffer, sizeof(long_buffer), "%s", message);
  text_layer_set_text(s_long_text, long_buffer);
}

static void create_weather_status_layer(Layer *parent_layer) {
  int coord_height = 15;
  s_weather_status_layer = layer_create(GRect(0, 84, 144, 84));

  s_weather_status_text = text_layer_create(GRect(0, 56, 144, 25));
  text_layer_set_background_color(s_weather_status_text, GColorClear);
  text_layer_set_text_color(s_weather_status_text, GColorWhite);
  text_layer_set_text_alignment(s_weather_status_text, GTextAlignmentCenter);
  text_layer_set_text(s_weather_status_text, "Loading...");

  layer_add_child(s_weather_status_layer, text_layer_get_layer(s_weather_status_text));

  s_lat_text = text_layer_create(GRect(4, 84 - coord_height, 68, coord_height));
  text_layer_set_background_color(s_lat_text, GColorClear);
  text_layer_set_text_color(s_lat_text, GColorWhite);
  text_layer_set_text_alignment(s_lat_text, GTextAlignmentRight);
  text_layer_set_font(s_lat_text, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_lat_text, "");

  layer_add_child(s_weather_status_layer, text_layer_get_layer(s_lat_text));

  s_long_text = text_layer_create(GRect(76, 84 - coord_height, 68, coord_height));
  text_layer_set_background_color(s_long_text, GColorClear);
  text_layer_set_text_color(s_long_text, GColorWhite);
  text_layer_set_text_alignment(s_long_text, GTextAlignmentRight);
  text_layer_set_font(s_long_text, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_long_text, "");

  layer_add_child(s_weather_status_layer, text_layer_get_layer(s_long_text));

  layer_add_child(parent_layer, s_weather_status_layer);
}

static void create_weather_layer(Layer *parent_layer) {
  int gutter = 16;
  s_temperature_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_26));

  s_temperature_layer = text_layer_create(GRect(72 + gutter / 2, 105, 72 - gutter, 30));
  text_layer_set_background_color(s_temperature_layer, GColorClear);
  text_layer_set_text_color(s_temperature_layer, GColorWhite);
  text_layer_set_text_alignment(s_temperature_layer, GTextAlignmentLeft);
  text_layer_set_font(s_temperature_layer, s_temperature_font);
  text_layer_set_text(s_temperature_layer, "");

  layer_add_child(parent_layer, text_layer_get_layer(s_temperature_layer));

  s_conditions_id_layer = text_layer_create(GRect(gutter / 2, 106, 72 - gutter / 2, 30));
  text_layer_set_background_color(s_conditions_id_layer, GColorClear);
  text_layer_set_text_color(s_conditions_id_layer, GColorWhite);
  text_layer_set_text_alignment(s_conditions_id_layer, GTextAlignmentRight);
  text_layer_set_font(s_conditions_id_layer, s_temperature_font);
  text_layer_set_text(s_conditions_id_layer, "");

  layer_add_child(parent_layer, text_layer_get_layer(s_conditions_id_layer));

  s_conditions_layer = text_layer_create(GRect(gutter / 2, 130, 72 - gutter / 2, 35));
  text_layer_set_background_color(s_conditions_layer, GColorClear);
  text_layer_set_text_color(s_conditions_layer, GColorWhite);
  text_layer_set_text_alignment(s_conditions_layer, GTextAlignmentRight);
  text_layer_set_text(s_conditions_layer, "");

  layer_add_child(parent_layer, text_layer_get_layer(s_conditions_layer));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  window_set_background_color(window, GColorBlack);
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_52));

  s_time_layer = text_layer_create(GRect(0, 0, 144, 55));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  s_date_layer = text_layer_create(GRect(0, 55, 144, 25));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));

  create_weather_status_layer(window_layer);
  create_weather_layer(window_layer);

  update_time();
}

static void update_temperature(char *temperature) {
  static char temperature_buffer[8];

  snprintf(temperature_buffer, sizeof(temperature_buffer), "%s", temperature);
  text_layer_set_text(s_temperature_layer, temperature_buffer);
}

static void update_conditions_id(char *conditions_id) {
  static char conditions_id_buffer[8];

  snprintf(conditions_id_buffer, sizeof(conditions_id_buffer), "%s", conditions_id);
  text_layer_set_text(s_conditions_id_layer, conditions_id_buffer);
}

static void update_conditions(char *message) {
  static char conditions_buffer[32];

  snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", message);
  text_layer_set_text(s_conditions_layer, conditions_buffer);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_LOG:
      update_status(t->value->cstring);
      break;
    case KEY_TEMPERATURE:
      update_temperature(t->value->cstring);
      break;
    case KEY_CONDITIONS:
      update_conditions(t->value->cstring);
      break;
    case KEY_CONDITIONS_ID:
      update_conditions_id(t->value->cstring);
      break;
    case KEY_LAT:
      update_lat(t->value->cstring);
      break;
    case KEY_LONG:
      update_long(t->value->cstring);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
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

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_temperature_font);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  app_message_register_inbox_received(inbox_received_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
