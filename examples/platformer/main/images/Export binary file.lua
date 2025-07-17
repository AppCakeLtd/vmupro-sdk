-- Export binary file
-- Thanks to Jerzy Kut for the num_to_char function
-- Modified 2025 8BM to:
-- - auto select adjustment of 0
-- - disable example script dialogue
-- - export all layers

function num_to_char ( number )
 return ( string.char ( math.mod ( math.mod ( number, 256 ) + 256, 256 ) ) )
end

function writeIntLSB ( file, number )
 file:write ( num_to_char( number )) -- x>>0
 file:write ( num_to_char( number / 256 )) -- x>>8
 file:write ( num_to_char( number / 65536 )) -- x>>16
 file:write ( num_to_char( number / 16777216 )) -- x>>24
end

function writeu8 ( file, number )
 file:write ( num_to_char( number )) -- x>>0
end

function stripExtension(filename)
    local length = string.len(filename)
    for i = length, 1, -1 do
        if string.sub(filename, i, i) == "." then
            return string.sub(filename, 1, i - 1)
        end
    end
    return filename
end


function main ()
 -- if mappy.msgBox ("Export Binary", "This example script will export the current layer as a binary file (CDXMap format) (anims are replaced with block 0)\nThis is the same as the default .map format when you save a .map file\n\nRun the script (you will be prompted for a filename to save as)?", mappy.MMB_OKCANCEL, mappy.MMB_ICONQUESTION) == mappy.MMB_OK then

 local mapName = mappy.getValue(mappy.MAPFILENAME) 
 local noExt = stripExtension(mapName)
 local exampleLayerNum = 0
 local nameAndLayer = noExt.."_layer_"..string.format("%01d", exampleLayerNum)
 local withExt = nameAndLayer..".map"

 local doSave = mappy.msgBox ("Export Binary", "Will save sequentially to "..withExt, mappy.MMB_OKCANCEL, mappy.MMB_ICONQUESTION) == mappy.MMB_OK
 -- local dosave = true
 if doSave then

  local w = mappy.getValue(mappy.MAPWIDTH)
  local h = mappy.getValue(mappy.MAPHEIGHT)

  if (w == 0) then
   mappy.msgBox ("Export binary file", "You need to load or create a map first", mappy.MMB_OK, mappy.MMB_ICONINFO)
  else

   --local isok,adjust = mappy.doDialogue ("Export binary file", "Adjust exported values by:", "-1", mappy.MMB_DIALOGUE1)
   --if isok == mappy.MMB_OK then

   local l = 0
   while l < mappy.getValue(mappy.NUMLAYERS) do

    local asname = noExt.."_layer_"..string.format("%01d",l)..".map"

    adjust = 0
-- open file as binary
    outas = io.open (asname, "wb")
    writeIntLSB (outas, w)
    writeIntLSB (outas, h)
    local y = 0
    while y < h do
     local x = 0
     while x < w do
      local mapval = mappy.getBlockValue (mappy.getBlock (x, y), mappy.BLKBG)
      mapval = mapval + adjust
      if mapval < 0 then
       mapval = 0
      end
      writeu8 (outas, mapval)
      x = x + 1
     end
     y = y + 1
    end
    outas:close ()
    l = l + 1
   end

  end
 end
end

test, errormsg = pcall( main )
if not test then
    mappy.msgBox("Error ...", errormsg, mappy.MMB_OK, mappy.MMB_ICONEXCLAMATION)
end
