;Author: Liu Junnan

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

@mat = global [4 x i32] [i32 0, i32 1, i32 1, i32 1], align 16
@n = global i32 0, align 4
@MOD = global i32 10007, align 4

define i32 @solve(i32 %x)
{
  %x.addr = alloca i32, align 4
  store i32 %x, i32* %x.addr
  %p = alloca i32, align 4
  %q = alloca i32, align 4
  %r = alloca i32, align 4
  %s = alloca i32, align 4
  %ans = alloca [2 x i32], align 4
  %arrIdx0 = getelementptr inbounds [2 x i32], [2 x i32]* %ans, i32 0, i32 0
  %arrIdx1 = getelementptr inbounds [2 x i32], [2 x i32]* %ans, i32 0, i32 1
  store i32 0, i32* %arrIdx0, align 4
  store i32 1, i32* %arrIdx1, align 4
  br label %for.cond0

for.cond0:
  %t1 = load i32, i32* %x.addr, align 4
  %t2 = icmp ne i32 %t1, 0
  br i1 %t2, label %for.body0, label %for.next0

for.body0:
  %t3 = load i32, i32* %x.addr, align 4
  %t4 = and i32 %t3, 1
  %t5 = icmp ne i32 %t4, 0
  br i1 %t5, label %if.then0, label %if.end0

if.then0:
  %t6 = getelementptr inbounds [2 x i32], [2 x i32]* %ans, i32 0, i32 0
  %t7 = load i32, i32* %t6, align 4
  %t8 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 0
  %t9 = load i32, i32* %t8, align 4
  %t10 = mul nsw i32 %t7, %t9
  %t11 = getelementptr inbounds [2 x i32], [2 x i32]* %ans, i32 0, i32 1
  %t12 = load i32, i32* %t11, align 4
  %t13 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 2
  %t14 = load i32, i32* %t13, align 4
  %t15 = mul nsw i32 %t12, %t14
  %t16 = add nsw i32 %t10, %t15
  store i32 %t16, i32* %p, align 4
  %t17 = getelementptr inbounds [2 x i32], [2 x i32]* %ans, i32 0, i32 0
  %t18 = load i32, i32* %t17, align 4
  %t19 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 1
  %t20 = load i32, i32* %t19, align 4
  %t21 = mul nsw i32 %t18, %t20
  %t22 = getelementptr inbounds [2 x i32], [2 x i32]* %ans, i32 0, i32 1
  %t23 = load i32, i32* %t22, align 4
  %t24 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 3
  %t25 = load i32, i32* %t24, align 4
  %t26 = mul nsw i32 %t23, %t25
  %t27 = add nsw i32 %t21, %t26
  store i32 %t27, i32* %q, align 4
  %t28 = load i32, i32* %p, align 4
  %t29 = load i32, i32* @MOD, align 4
  %t30 = srem i32 %t28, %t29
  %t31 = getelementptr inbounds [2 x i32], [2 x i32]* %ans, i32 0, i32 0
  %t32 = load i32, i32* %t31, align 4
  store i32 %t30, i32* %t31, align 4
  %t33 = load i32, i32* %q, align 4
  %t34 = load i32, i32* @MOD, align 4
  %t35 = srem i32 %t33, %t34
  %t36 = getelementptr inbounds [2 x i32], [2 x i32]* %ans, i32 0, i32 1
  %t37 = load i32, i32* %t36, align 4
  store i32 %t35, i32* %t36, align 4
  br label %if.end0

if.end0:
  %t38 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 0
  %t39 = load i32, i32* %t38, align 4
  %t40 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 0
  %t41 = load i32, i32* %t40, align 4
  %t42 = mul nsw i32 %t39, %t41
  %t43 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 1
  %t44 = load i32, i32* %t43, align 4
  %t45 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 2
  %t46 = load i32, i32* %t45, align 4
  %t47 = mul nsw i32 %t44, %t46
  %t48 = add nsw i32 %t42, %t47
  store i32 %t48, i32* %p, align 4
  %t49 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 0
  %t50 = load i32, i32* %t49, align 4
  %t51 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 1
  %t52 = load i32, i32* %t51, align 4
  %t53 = mul nsw i32 %t50, %t52
  %t54 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 1
  %t55 = load i32, i32* %t54, align 4
  %t56 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 3
  %t57 = load i32, i32* %t56, align 4
  %t58 = mul nsw i32 %t55, %t57
  %t59 = add nsw i32 %t53, %t58
  store i32 %t59, i32* %q, align 4
  %t60 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 2
  %t61 = load i32, i32* %t60, align 4
  %t62 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 0
  %t63 = load i32, i32* %t62, align 4
  %t64 = mul nsw i32 %t61, %t63
  %t65 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 3
  %t66 = load i32, i32* %t65, align 4
  %t67 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 2
  %t68 = load i32, i32* %t67, align 4
  %t69 = mul nsw i32 %t66, %t68
  %t70 = add nsw i32 %t64, %t69
  store i32 %t70, i32* %r, align 4
  %t71 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 1
  %t72 = load i32, i32* %t71, align 4
  %t73 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 2
  %t74 = load i32, i32* %t73, align 4
  %t75 = mul nsw i32 %t72, %t74
  %t76 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 3
  %t77 = load i32, i32* %t76, align 4
  %t78 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 3
  %t79 = load i32, i32* %t78, align 4
  %t80 = mul nsw i32 %t77, %t79
  %t81 = add nsw i32 %t75, %t80
  store i32 %t81, i32* %s, align 4
  %t82 = load i32, i32* %p, align 4
  %t83 = load i32, i32* @MOD, align 4
  %t84 = srem i32 %t82, %t83
  %t85 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 0
  %t86 = load i32, i32* %t85, align 4
  store i32 %t84, i32* %t85, align 4
  %t87 = load i32, i32* %q, align 4
  %t88 = load i32, i32* @MOD, align 4
  %t89 = srem i32 %t87, %t88
  %t90 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 1
  %t91 = load i32, i32* %t90, align 4
  store i32 %t89, i32* %t90, align 4
  %t92 = load i32, i32* %r, align 4
  %t93 = load i32, i32* @MOD, align 4
  %t94 = srem i32 %t92, %t93
  %t95 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 2
  %t96 = load i32, i32* %t95, align 4
  store i32 %t94, i32* %t95, align 4
  %t97 = load i32, i32* %s, align 4
  %t98 = load i32, i32* @MOD, align 4
  %t99 = srem i32 %t97, %t98
  %t100 = getelementptr inbounds [4 x i32], [4 x i32]* @mat, i32 0, i32 3
  %t101 = load i32, i32* %t100, align 4
  store i32 %t99, i32* %t100, align 4
  %t102 = load i32, i32* %x.addr, align 4
  %t103 = ashr i32 %t102, 1
  store i32 %t103, i32* %x.addr, align 4
  br label %for.cond0

for.next0:
  %t104 = getelementptr inbounds [2 x i32], [2 x i32]* %ans, i32 0, i32 0
  %t105 = load i32, i32* %t104, align 4
  ret i32 %t105
}

define i32 @main()
{
  %t1 = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32* @n)
  %t2 = load i32, i32* @n, align 4
  %t3 = call i32 @solve(i32 %t2)
  %t4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32 %t3)
  ret i32 0
}
declare i32 @__isoc99_scanf(i8*, ...)

declare i32 @printf(i8*, ...)
