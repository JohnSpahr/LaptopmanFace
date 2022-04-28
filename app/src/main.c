/*
  L A P T O P M A N - Pebble Watch Face

  Created by John Spahr (johnspahr.org)

  Thanks to: "PebbleFaces" example, the watch face creation guide hosted by Rebble, the Rebble team, and all of you for using my stupid creation.

  One more thing: The Laptopman is my own work of art, perhaps even my magnum opus. It's pretty great.
*/
#include <pebble.h>

static Window *window;            // window object
static BitmapLayer *bitmap_layer; // layer for display the bitmap
static GBitmap *current_bmp;      // bitmap object
static TextLayer *s_time_layer;   // the clock (text layer)

static GBitmap *mono_laptopman;  // mono laptopman bitmap
static GBitmap *color_laptopman; // color laptopman bitmap
static GBitmap *mono_angry;      // mono angry laptopman bitmap
static GBitmap *color_angry;     // color angry laptopman bitmap

static void update_time()
{
  // get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  // display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  update_time(); // update the time on timer tick
}

static void show_laptopman()
{
  // show default laptopman bitmap
  switch (PBL_PLATFORM_TYPE_CURRENT)
  {
  // detect platform...
  case PlatformTypeAplite:
    // monochrome pebbles
    mono_laptopman = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MONO_LAPTOPMAN); // create bitmap from image resource
    bitmap_layer_set_bitmap(bitmap_layer, mono_laptopman);                           // set bitmap layer image

    if (current_bmp)
    {
      // destroy current bitmap object if it exists
      gbitmap_destroy(current_bmp);
      current_bmp = NULL;
    }

    current_bmp = mono_laptopman; // update current bitmap object
    break;
  case PlatformTypeBasalt:
    // color pebbles
    color_laptopman = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_COLOR_LAPTOPMAN); // create bitmap from image resource
    bitmap_layer_set_bitmap(bitmap_layer, color_laptopman);                            // set bitmap layer image

    if (current_bmp)
    {
      // destroy current bitmap object if it exists
      gbitmap_destroy(current_bmp);
      current_bmp = NULL;
    }

    current_bmp = color_laptopman; // update current bitmap object
    break;
  }
}

static void window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window); // get current window layer
  GRect bounds = layer_get_bounds(window_layer);       // get screen bounds

  bitmap_layer = bitmap_layer_create(bounds);                          // create bitmap layer
  layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer)); // add bitmap layer to window
  current_bmp = NULL;                                                  // set bitmap object to null

  // create clock text layer...
  s_time_layer = text_layer_create(
      GRect(0, 25, bounds.size.w, bounds.size.h));

  // time text layer setup...
  text_layer_set_background_color(s_time_layer, GColorClear);                        // clear background color
  text_layer_set_text_color(s_time_layer, GColorWhite);                              // white text color
  text_layer_set_text(s_time_layer, "00:00");                                        // set text to 00:00
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD)); // set font to Gothic, 28pt, bold
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);                 // center align text

  // add clock text layer to window layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  // load default laptopman bitmap
  show_laptopman();
}

static void window_unload(Window *window)
{
  // get rid of bitmap layer
  bitmap_layer_destroy(bitmap_layer);

  // dispose of bitmap itself
  gbitmap_destroy(current_bmp);

  // destroy time layer
  text_layer_destroy(s_time_layer);
}

void tap_handler(AccelAxisType accel, int32_t direction)
{
  // when watch is shaken, determine which bitmap is showing
  if (current_bmp != mono_laptopman && current_bmp != color_laptopman)
  {
    show_laptopman(); // show default laptopman bitmap if angry bitmap is currently showing
  }
  else
  {
    // otherwise, show angry laptopman bitmap
    switch (PBL_PLATFORM_TYPE_CURRENT)
    {
    // detect platform...
    case PlatformTypeAplite:
      // monochrome pebbles
      mono_angry = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MONO_ANGRY); // create bitmap from image resource
      bitmap_layer_set_bitmap(bitmap_layer, mono_angry);                       // set bitmap layer image

      if (current_bmp)
      {
        // destroy current bitmap object if it exists
        gbitmap_destroy(current_bmp);
        current_bmp = NULL;
      }

      current_bmp = mono_angry; // update current bitmap object
      break;
    case PlatformTypeBasalt:
      // color pebbles
      color_angry = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_COLOR_ANGRY); // create bitmap from image resource
      bitmap_layer_set_bitmap(bitmap_layer, color_angry); // set bitmap layer image

      if (current_bmp)
      {
        // destroy current bitmap object if it exists
        gbitmap_destroy(current_bmp);
        current_bmp = NULL;
      }

      current_bmp = color_angry; // update current bitmap object
      break;
    }
  }
}

static void init(void)
{
  window = window_create(); // create a new window. woohoo!
  window_set_window_handlers(window, (WindowHandlers){
                                         .load = window_load,
                                         .unload = window_unload,
                                     }); // set functions for window handlers

  window_stack_push(window, true); // push window

  accel_tap_service_subscribe(tap_handler); // set tap_handler function as handler for watch shake

  update_time(); // register with TickTimerService

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler); // set tick_handler function as time handler (start keeping time, essentially. kinda important on a watch.)
}

static void deinit(void)
{
  window_destroy(window); // literally obliterate the window
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}