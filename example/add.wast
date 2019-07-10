(module
    (export "add3" (func $add3))
    (export "add2" (func $add2))
    (func $add2 (param $a i32) (param $b i32) (result i32)
        (i32.add
            (local.get $a)
            (local.get $b)
        )
    )
    (import "env" "importfunc" (func $importfunc))
    (func $inc (param $a i32) (result i32)
        (i32.add
            (local.get $a)
            (i32.const 1)
        )
    )
    (func $add3 (param $a i32) (param $b i32) (param $c i32) (result i32)
        (i32.add
            (i32.add
                (local.get $a)
                (local.get $b)
            )
            (local.get $c)
        )
    )
)
