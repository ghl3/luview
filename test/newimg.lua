
local lunum = require 'lunum'
local luview = require 'luview'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local shade = shaders.load_shader("phong")
local image = luview.ImagePlane()
local lumsrc = luview.DataSource()
local rgbsrc = luview.DataSource()
local lut = luview.DataSource()
local cmshade = shaders.load_shader("cbar")

local cbar = require 'cbar'
local lutdata = lunum.array(cbar):resize{256,4}

lut:set_data(lutdata)
lut:set_mode("rgba")

window:set_color(0,0,0)
box:set_alpha(1.0)
box:set_linewidth(2.0)
box:set_shader(shade)
box:set_color(0.2, 0.2, 0.0)

local lumdata = lunum.fromfile("data/rhoJ.bin"):resize{1024,1024} / 3.0

lumsrc:set_mode("luminance")
lumsrc:set_data(lumdata)

image:set_data("color_table", lut)
image:set_data("image", lumsrc)
image:set_alpha(1.0)
image:set_orientation(0, 0, -90)
image:set_shader(cmshade)


while window:render_scene{box, image} == "continue" do end
