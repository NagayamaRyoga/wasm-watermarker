#!/usr/bin/env ruby
require 'fileutils'
require 'pathname'

SNPI="./build/bin/snpi"
PISN="./build/bin/pisn"

WASM_BINARIES = {
  :source_map => "./node_modules/source-map/lib/mappings.wasm",
  :wasm_flate => "./node_modules/wasm-flate/wasm_flate_bg.wasm",
  :ammo => "./node_modules/ammo.js/builds/ammo.wasm.wasm",
  :jq => "./node_modules/jq-web/jq.wasm.wasm",
  :vim_small => "./node_modules/vim-wasm/small/vim.wasm",
  :vim => "./node_modules/vim-wasm/vim.wasm",
}

OUTPUT_DIRS = {
  :source_map => "./out/source-map",
  :wasm_flate => "./out/wasm-flate",
  :ammo => "./out/ammo.js",
  :jq => "./out/jq-web",
  :vim => "./out/vim-wasm",
  :vim_small => "./out/vim-wasm/small",
}

METHODS = [
  {:name => "function-reorder", :abbr => "fn"},
  {:name => "export-reorder", :abbr => "ex"},
  {:name => "operand-swap", :abbr => "op"},
]

watermark = "N7AStlK2gioIqeRi"

def copy(file, out)
    FileUtils.copy(file, out, {:verbose => true})
end

def embed(file, out, method, watermark, chunk_size = 20)
  output = IO.popen([SNPI, "-o", out.to_s, "-m", method, "-w", watermark, "-c", chunk_size.to_s, file.to_s]).read

  puts "#{file}, #{out}, #{method}, #{watermark}, #{output}"
end

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
  end
end
