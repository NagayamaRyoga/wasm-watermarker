#!/usr/bin/env ruby
require_relative './common'

watermark = WATERMARK

METHODS.each do |method|
  WASM_BINARIES.each do |id, file|
    dir = Pathname(OUTPUT_DIRS[id])
    out = dir + Pathname(file).basename.sub(".wasm", "-#{method[:abbr]}-#{watermark}.wasm")

    extract(out, method[:name])

    OPT_PASSES.each {|pass| extract(out.sub(".wasm", "-#{pass}.wasm"), method[:name])}
  end
end
