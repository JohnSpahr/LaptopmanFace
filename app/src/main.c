/*
  L A P T O P M A N - Pebble Watch Face

  Created by John Spahr (johnspahr.org)

  Thanks to: "PebbleFaces" example, the watch face creation guide hosted by Rebble, the Rebble team, Robert Smith, and all of you for using my first watch face!

  One more thing: The Laptopman is my own work of art, perhaps even my magnum opus. It's pretty great.
*/
#include <pebble.h>

static Window *window;            // window object
static TextLayer *s_time_layer;   // the clock (text layer)
static BitmapLayer *bitmap_layer; // layer for display the bitmap
static GBitmap *current_bmp;      // current bitmap object
static GBitmap *mono_laptopman;   // mono laptopman bitmap
static GBitmap *color_laptopman;  // color laptopman bitmap
static GBitmap *round_laptopman;  // round laptopman bitmap
static GBitmap *mono_angry;       // mono angry laptopman bitmap
static GBitmap *color_angry;      // color angry laptopman bitmap
static GBitmap *round_angry;      // round angry laptopman bitmap

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
  case PlatformTypeDiorite:
    // monochrome pebbles
    if (!mono_laptopman)
    {
      mono_laptopman = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MONO_LAPTOPMAN); // create bitmap from image resource if it doesn't exist yet
    }
    bitmap_layer_set_bitmap(bitmap_layer, mono_laptopman); // set bitmap layer image
    current_bmp = mono_laptopman;                          // update current bitmap object
    break;
  case PlatformTypeBasalt:
    // color (rectangular) pebbles
    bitmap_layer_set_background_color(bitmap_layer, GColorLiberty); // set blue background color for bitmap layer
    if (!color_laptopman)
    {
      color_laptopman = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_COLOR_LAPTOPMAN); // create bitmap from image resource if it doesn't exist yet
    }
    bitmap_layer_set_bitmap(bitmap_layer, color_laptopman); // set bitmap layer image
    current_bmp = color_laptopman;                          // update current bitmap object
    break;
  case PlatformTypeChalk:
    // pebble time round
    bitmap_layer_set_background_color(bitmap_layer, GColorLiberty); // set blue background color for bitmap layer
    if (!round_laptopman)
    {
      round_laptopman = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ROUND_LAPTOPMAN); // create bitmap from image resource if it doesn't exist yet
    }
    bitmap_layer_set_bitmap(bitmap_layer, round_laptopman); // set bitmap layer image
    current_bmp = round_laptopman;                          // update current bitmap object
    break;
  }
}

static void bluetooth_callback(bool connected)
{
  if (connected)
  {
    show_laptopman(); // show nice laptopman if connected
  }
  else
  {
    // otherwise, show angry laptopman bitmap
    vibes_double_pulse(); // issue a vibrating alert on disconnect

    switch (PBL_PLATFORM_TYPE_CURRENT)
    {
    // detect platform...
    case PlatformTypeAplite:
    case PlatformTypeDiorite:
      // monochrome pebbles
      if (!mono_angry)
      {
        mono_angry = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MONO_ANGRY); // create bitmap from image resource if it doesn't exist yet
      }
      bitmap_layer_set_bitmap(bitmap_layer, mono_angry); // set bitmap layer image
      current_bmp = mono_angry;                          // update current bitmap object
      break;
    case PlatformTypeBasalt:
      // color (rectangular) pebbles
      bitmap_layer_set_background_color(bitmap_layer, GColorDarkCandyAppleRed); // set red background color for bitmap layer
      if (!color_angry)
      {
        color_angry = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_COLOR_ANGRY); // create bitmap from image resource if it doesn't exist yet
      }
      bitmap_layer_set_bitmap(bitmap_layer, color_angry); // set bitmap layer image
      current_bmp = color_angry;                          // update current bitmap object
      break;
    case PlatformTypeChalk:
      // pebble time round
      bitmap_layer_set_background_color(bitmap_layer, GColorDarkCandyAppleRed); // set red background color for bitmap layer
      if (!round_angry)
      {
        round_angry = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ROUND_ANGRY); // create bitmap from image resource if it doesn't exist yet
      }
      bitmap_layer_set_bitmap(bitmap_layer, round_angry); // set bitmap layer image
      current_bmp = round_angry;                          // update current bitmap object
      break;
    }
  }
}

static void window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window); // get current window layer
  GRect bounds = layer_get_bounds(window_layer);       // get screen bounds

  bitmap_layer = bitmap_layer_create(bounds);                          // create bitmap layer
  layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer)); // add bitmap layer to window

  // create clock text layer...
  s_time_layer = text_layer_create(
      GRect(PBL_IF_COLOR_ELSE(3, 0), PBL_IF_ROUND_ELSE(26, 24), bounds.size.w, bounds.size.h));

  // time text layer setup...
  text_layer_set_background_color(s_time_layer, GColorClear);                        // set clear background color
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

  // dispose of bitmaps if they exist...
  if (current_bmp)
  {
    gbitmap_destroy(current_bmp); // current bitmap
  }

  // destroy time text layer
  text_layer_destroy(s_time_layer);
}

static void init(void)
{
  window = window_create(); // create a new window. woohoo!
  window_set_window_handlers(window, (WindowHandlers){
                                         .load = window_load,
                                         .unload = window_unload,
                                     }); // set functions for window handlers

  window_stack_push(window, true); // push window

  update_time(); // register with TickTimerService

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler); // set tick_handler function as time handler (start keeping time, essentially. kinda important on a watch.)

  // register for Bluetooth connection updates
  connection_service_subscribe((ConnectionHandlers){
      .pebble_app_connection_handler = bluetooth_callback});
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