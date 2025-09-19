--- @file vmupro_layers.lua
--- @brief VMU Pro LUA SDK - Layer System Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-19
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Layer system and compositing functions for VMU Pro LUA applications.
--- All layer functions are available globally in the LUA environment.

--- @brief Create a new layer
--- @param width number Layer width in pixels
--- @param height number Layer height in pixels
--- @return number Layer ID for use with other layer functions
--- @usage local layer_id = vmupro_layer_create(240, 240)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_layer_create(width, height) end

--- @brief Destroy a layer and free its memory
--- @param layer_id number Layer ID returned from vmupro_layer_create
--- @usage vmupro_layer_destroy(layer_id)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_layer_destroy(layer_id) end

--- @brief Set the scroll position for a layer
--- @param layer_id number Layer ID
--- @param scroll_x number Horizontal scroll offset
--- @param scroll_y number Vertical scroll offset
--- @usage vmupro_layer_set_scroll(layer_id, 10, 20)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_layer_set_scroll(layer_id, scroll_x, scroll_y) end

--- @brief Set the rendering priority for a layer (higher = on top)
--- @param layer_id number Layer ID
--- @param priority number Layer priority (0-255)
--- @usage vmupro_layer_set_priority(layer_id, 100)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_layer_set_priority(layer_id, priority) end

--- @brief Set the alpha transparency level for a layer
--- @param layer_id number Layer ID
--- @param alpha number Alpha level (0-255, where 0 is transparent and 255 is opaque)
--- @usage vmupro_layer_set_alpha(layer_id, 128) -- 50% transparent
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_layer_set_alpha(layer_id, alpha) end

--- @brief Blit a background buffer to a layer
--- @param layer_id number Layer ID
--- @param buffer userdata Buffer to blit to the layer
--- @usage vmupro_layer_blit_background(layer_id, background_buffer)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_layer_blit_background(layer_id, buffer) end

--- @brief Render all layers to the screen
--- @usage vmupro_render_all_layers()
--- @note Call this after setting up all layers to composite them to the screen
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_render_all_layers() end

--- @brief Blend two layers using additive blending
--- @param layer1 userdata First layer buffer
--- @param layer2 userdata Second layer buffer
--- @param output userdata Output layer buffer
--- @param width number Buffer width
--- @param height number Buffer height
--- @usage vmupro_blend_layers_additive(layer1_buffer, layer2_buffer, output_buffer, 240, 240)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blend_layers_additive(layer1, layer2, output, width, height) end

--- @brief Blend two layers using multiply blending
--- @param layer1 userdata First layer buffer
--- @param layer2 userdata Second layer buffer
--- @param output userdata Output layer buffer
--- @param width number Buffer width
--- @param height number Buffer height
--- @usage vmupro_blend_layers_multiply(layer1_buffer, layer2_buffer, output_buffer, 240, 240)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blend_layers_multiply(layer1, layer2, output, width, height) end

--- @brief Blend two layers using screen blending
--- @param layer1 userdata First layer buffer
--- @param layer2 userdata Second layer buffer
--- @param output userdata Output layer buffer
--- @param width number Buffer width
--- @param height number Buffer height
--- @usage vmupro_blend_layers_screen(layer1_buffer, layer2_buffer, output_buffer, 240, 240)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blend_layers_screen(layer1, layer2, output, width, height) end