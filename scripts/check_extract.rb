#!/usr/bin/env ruby
require_relative './common'

watermark = WATERMARK

METHODS.each do |method|
  WASM_BINARIES.each do |id, file|
    dir = Pathname(OUTPUT_DIRS[id])
    a = dir + Pathname(file).basename.sub(".wasm", "-#{method[:abbr]}-#{watermark}.wasm")

    OPT_PASSES.each do |pass|
      b = a.sub(".wasm", "-#{pass}.wasm")
      puts "#{a.basename} - #{b.basename}: #{method[:name]}, #{pass}, #{FileUtils.cmp(a, b)}"
    end
  end
end
