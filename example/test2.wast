(module
    (export "f1" (func $f1))
    (export "f2" (func $f2))
    (export "f3" (func $f3))
    (func $f1 (result i32)
        (i32.add
            (i32.const 1)
            (i32.const 2)
        )
    )
    (func $f2 (result i32)
        (i32.add
            (i32.add
                (i32.const 1)
                (i32.const 2)
            )
            (i32.const 3)
        )
    )
    (func $f3 (result i32)
        (i32.add
            (i32.add
                (i32.const 1)
                (i32.const 2)
            )
            (i32.add
                (i32.const 4)
                (i32.const 3)
            )
        )
    )
)
