#include <pebble.h>

static Window *window;
static TextLayer *s_time_layer;
static GFont s_time_font;

static void click_config_provider(void *context) {

}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char buffer[] = "00:00";

  if (clock_is_24h_style() == true) {
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    if (tick_time->tm_hour % 12 < 10) {
      strftime(buffer, sizeof("00:00"), "%l:%M", tick_time);
    } else {
      strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    }
  }

  text_layer_set_text(s_time_layer, buffer);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  window_set_background_color(window, GColorBlack);

  s_time_layer = text_layer_create(GRect(0, 10, 144, 70));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_52));
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  update_time();

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  fonts_unload_custom_font(s_time_font);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

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
