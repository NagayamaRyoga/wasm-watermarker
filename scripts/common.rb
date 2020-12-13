require 'fileutils'
require 'pathname'

SNPI = "./build/bin/snpi"
PISN = "./build/bin/pisn"

WASM_OPT = "./build/binaryen/build/bin/wasm-opt"

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

OPT_PASSES = [
  "coalesce-locals",
  "code-folding",
  "code-pushing",
  "dae-optimizing",
  "dce",
  "directize",
  "duplicate-function-elimination",
  "duplicate-import-elimination",
  "flatten",
  "generate-stack-ir",
  "inlining-optimizing",
  # "local-cse", # must be --flatten before
  "memory-packing",
  "merge-blocks",
  "merge-locals",
  # "optimize-added-constants-propagate", # Assertion failed
  # "optimize-added-constants", # Assertion failed
  "optimize-instructions",
  "optimize-stack-ir",
  "pick-load-signs",
  "precompute-propagate",
  "precompute",
  "remove-unused-brs",
  "remove-unused-module-elements",
  "remove-unused-names",
  "reorder-locals",
  "rse",
  "simplify-globals-optimizing",
  "simplify-globals",
  "simplify-locals-nostructure",
  "simplify-locals",
  "ssa-nomerge",
  "vacuum",
]

WATERMARK = "N7AStlK2gioIqeRi"

def copy(file, out)
    FileUtils.copy(file, out, {:verbose => true})
end

def embed(file, out, method, watermark, chunk_size = 20)
  output = IO.popen([SNPI, "-o", out.to_s, "-m", method, "-w", watermark, "-c", chunk_size.to_s, file.to_s]).read

  puts "embed #{file}, #{out}, #{method}, #{watermark}, #{output}"
end

def optimize(file, out, pass)
  output = IO.popen([WASM_OPT, "-o", out.to_s, "--#{pass}", file.to_s]).read

  puts "optimize #{file}, #{out}, #{pass}, #{output}"
end

def extract(file, method)
  out = Pathname(file).sub_ext(".txt")

  output = IO.popen([PISN, "-m", method, file.to_s]).read
  File.write(out, output)

  puts "extract #{file}, #{method}, #{out}"
end
