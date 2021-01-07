#!/usr/bin/env ruby
require_relative './common'
require 'amatch'

watermark = WATERMARK

METHODS.each do |method|
  WASM_BINARIES.each do |id, file|
    dir = Pathname(OUTPUT_DIRS[id])
    a = dir + Pathname(file).basename.sub(".wasm", "-#{method[:abbr]}-#{watermark}.wasm").sub_ext(".txt")

    OPT_PASSES.each do |pass|
      b = a.to_s.end_with?(".wasm.txt") ? a.sub(".wasm.txt", "-#{pass}.wasm.txt") : a.sub(".txt", "-#{pass}.txt")

      a_lines = File.readlines(a)
      b_lines = File.readlines(b)
      a_bits = a_lines[0].to_i
      b_bits = b_lines[0].to_i
      a_wm = [a_lines[1]].pack("H*").unpack("B*")[0][0, a_bits]
      b_wm = [b_lines[1]].pack("H*").unpack("B*")[0][0, b_bits]
      puts "#{a.basename}, #{b.basename}, #{method[:name]}, #{pass}, #{a_bits}, #{b_bits}, #{a_wm.levenshtein_similar(b_wm)}"
    end
  end if method[:name] != "null"
end
