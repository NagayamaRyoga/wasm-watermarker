#!/usr/bin/env ruby
require_relative './common'

watermark = WATERMARK

WASM_BINARIES.each do |id, file|
  dir = Pathname(OUTPUT_DIRS[id])

  FileUtils.mkdir_p(dir)
  copy(file, dir + Pathname(file).basename)
end

METHODS.each do |method|
  WASM_BINARIES.each do |id, file|
    dir = Pathname(OUTPUT_DIRS[id])
    out = dir + Pathname(file).basename.sub(".wasm", "-#{method[:abbr]}-#{watermark}.wasm")

    embed(file, out, method[:name], watermark)

    OPT_PASSES.each {|pass| optimize(out, out.sub(".wasm", "-#{pass}.wasm"), pass)}
  end
end
