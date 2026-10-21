# CMU-SAFARI LEAP / LEAP-BV

本仓库保存 CMU-SAFARI LEAP 的原始代码，并在原有 LEAP bit-vector 数据流旁边加入了 AVX512 后端和对应正确性测试。

LEAP 的核心路径保持不变：

```text
DNA 字符串
  -> bit0/bit1 bit-plane 表示
  -> shift
  -> XOR/OR mismatch mask
  -> VtH / next-1 search
  -> LEAP lane/state recurrence
```

代码库中存在一些历史 driver、Needleman-Wunsch/parasail 对比代码和 Bowtie2 augment 目录。核心 LEAP-BV 后端和本文档中的 AVX512 正确性测试不依赖 parasail。

## 环境要求

推荐 Linux + GCC：

```bash
g++ --version
make --version
python3 --version
```

需要的 CPU 指令集：

- SSE 路径：SSE/SSSE3/SSE4.2
- AVX2 路径：AVX2 + BMI
- AVX512 路径：AVX512F + AVX512BW + AVX512VL + BMI

可用下面命令检查本机是否支持 AVX512：

```bash
grep -m1 avx512 /proc/cpuinfo
```

默认 `Makefile` 中的 `CXX` 是历史值 `g++-5`。现代环境通常没有这个编译器，建议显式传入：

```bash
make CXX=g++ <target>
```

## 快速开始

清理并运行小型后端一致性测试：

```bash
make clean
make CXX=g++ test_all_backends_small
```

预期输出类似：

```text
PASS test_backend_consistency pairs=27 thresholds=0,1,2,3,5,8,10
```

构建 AVX512 driver：

```bash
make CXX=g++ vectorSHD_ED_avx512
make CXX=g++ vectorED_avx512
```

`vectorED_avx512` 不强制依赖 `parasail.h`。只有显式定义 `USE_PARASAIL` 时才包含 parasail 头文件。

## 编译变量和可选参数

所有目标都可以通过 Make 变量覆盖编译器：

```bash
make CXX=g++ <target>
```

常用变量/宏：

| 名称 | 用法 | 说明 |
| --- | --- | --- |
| `CXX=g++` | `make CXX=g++ vectorSHD_ED` | 使用系统默认 GCC C++ 编译器，避免历史默认值 `g++-5` 不存在 |
| `CFLAGS` | Makefile 内部变量 | 默认包含 `-O3 --std=c++11 -mbmi -mavx2 -msse4.2` 和本仓库所需 include path |
| `AVX512_FLAGS` | Makefile 内部变量 | 在 `CFLAGS` 基础上追加 `-DUSE_AVX512 -mavx512f -mavx512bw -mavx512vl` |
| `USE_AVX512` | AVX512 target 自动定义 | 启用 `__m512i` 后端代码和 512-bit correctness tests |
| `USE_PARASAIL` | 手动追加时使用 | 只在需要 parasail 对比代码时启用；核心 LEAP-BV 和 AVX512 tests 不需要 |

示例：

```bash
make CXX=g++ vectorSHD_ED
make CXX=g++ vectorSHD_ED_avx512
make CXX=g++ test_all_backends_small
```

运行 driver 时，第一个命令行参数通常是 edit-distance threshold，例如：

```bash
./vectorSHD_ED 2 < input.txt
./vectorSHD_ED_avx512 2 < input.txt
```

## 后端策略

### SSE backend

有效位宽：128 bp。

SSE 路径使用原有 128-bit LEAP-BV primitive：

- `sse_convert2bit`
- `shift_left_sse`
- `shift_right_sse`
- `bit_vec_filter_sse`

在小型后端一致性测试中，SSE runner 使用这些 SSE bit-plane / shift / mismatch-mask primitive，并在测试代码中执行与 LEAP 相同的 lane recurrence。这样可以直接比较 SSE、AVX2、AVX512 的 pass/fail 行为，而不引入 DP 或 ASCII 比较到生产路径。

### AVX2 / AVX256 backend

有效位宽：256 bp。

AVX2 是原始 `SIMD_ED` 的主要 SIMD 路径：

- `avx_convert2bit`
- `shift_left_avx`
- `shift_right_avx`
- `calculate_masks`
- `count_ID_length_avx`
- `SIMD_ED::run`

AVX2 后端仍然按单个 read/reference pair 的 bit-vector 计算，不把 SIMD lane 解释成不同 read/reference pair。

AVX2 的 `hamming_masks` 使用 32-byte aligned allocation，避免 `_mm256_*` aligned access 在某些环境下触发未对齐崩溃。

### AVX512 backend

有效位宽：512 bp。

AVX512 是在 AVX2 旁边新增的最小化后端，不改写 LEAP 算法：

- `avx512_convert2bit`
- `shift_left_avx512`
- `shift_right_avx512`
- `bit_vec_filter_avx512`
- `popcount_m512i_avx512`
- `popcount_SHD_avx512`
- `count_ID_length_avx512`

AVX512 后端只扩大同一个 read/reference pair 内部的 bit-vector 宽度到 512 bit，不做多 read batching。

`avx512_convert2bit` 是原始 `avx_convert2bit` 的宽度扩展：

- 输入 DNA 字符仍先转换成 `bit0/bit1` bit-plane。
- 512 bases 按与 AVX2 `BASE_SHIFT1` / `BASE_SHIFT2` 等价的 8-way transpose 顺序排列。
- 使用 `LOC_MASK_AVX512`，其 bit 顺序与 `LOC_MASK_AVX` 保持一致。
- packed byte equality 在 512-bit vector register 中完成，并生成 `__m512i` vector byte mask。
- `bit0` / `bit1` 通过 `__m512i` 的 AND/OR 累积得到。
- 不使用 `_mm512_cmpeq_epi8_mask` 或 `__mmask64` 作为 bit-plane 结果，也不从 ASCII 直接生成 lane mismatch mask。

AVX512 编译宏和 flags：

```text
-DUSE_AVX512 -mavx512f -mavx512bw -mavx512vl -mbmi
```

AVX512 `hamming_masks` 使用 64-byte aligned allocation，避免 ZMM aligned access 崩溃。

## 主要 Makefile 目标

### 原有/常用目标

```bash
make CXX=g++ vectorSHD_ED
make CXX=g++ vectorED
make CXX=g++ bit_convert
make CXX=g++ popcount
```

注意：

- `testNW` 是历史目标，链接 `-lparasail`，如果系统没有 parasail 会失败。
- `vectorED` 包含 Needleman-Wunsch 相关历史代码。
- 核心 LEAP-BV correctness 测试不需要 parasail。

### AVX512 目标

```bash
make CXX=g++ vectorSHD_ED_avx512
make CXX=g++ vectorED_avx512
```

### AVX512 correctness 测试

```bash
make CXX=g++ test_avx512_bit_convert
make CXX=g++ test_avx512_shift
make CXX=g++ test_avx512_lane_masks
make CXX=g++ test_avx512_vth
make CXX=g++ test_avx512_full_correctness
make CXX=g++ test_avx512_correctness
```

这些测试覆盖：

- DNA char 到 bit-plane conversion
- 512-bit continuous shift
- lane mismatch mask generation
- VtH / next-1 search
- full AVX512 LEAP pass/fail 与测试内 scalar Levenshtein oracle 的比较

### SSE / AVX2 / AVX512 小型后端一致性测试

数据文件：

```text
tests/data/small_read_ref_pairs.tsv
```

格式：

```text
read<TAB>reference<TAB>description
```

构建三个 runner：

```bash
make CXX=g++ backend_runner_sse
make CXX=g++ backend_runner_avx2
make CXX=g++ backend_runner_avx512
```

运行一致性测试：

```bash
make CXX=g++ test_backend_consistency
```

一键构建并运行：

```bash
make CXX=g++ test_all_backends_small
```

该测试会：

1. 读取 `tests/data/small_read_ref_pairs.tsv`
2. 对每个 pair 和阈值 `0,1,2,3,5,8,10` 计算测试内 scalar Levenshtein oracle
3. 分别运行 SSE、AVX2、AVX512 backend runner
4. 检查三个后端是否互相一致，并且是否等于 scalar oracle 的 pass/fail

## 输入格式和 driver 使用

`vectorSHD_ED` / `vectorSHD_ED_avx512` 从 stdin 读取 read/reference 成对输入，以 `end_of_file` 结束。示例：

```bash
cat > /tmp/leap_input.txt <<'EOF'
ACGTACGT
ACGTACGT
ACGTACGT
ACGTTCGT
end_of_file
EOF

./vectorSHD_ED_avx512 1 < /tmp/leap_input.txt
```

第一个参数是 edit-distance threshold。

可选择的运行方式：

| 目标/命令 | 后端 | 典型用途 |
| --- | --- | --- |
| `./vectorSHD_ED <E>` | AVX2/AVX256 默认路径 | 原始 LEAP-BV driver |
| `./vectorSHD_ED_avx512 <E>` | AVX512 路径 | 显式运行 512-bit LEAP-BV |
| `./vectorED_avx512 ...` | AVX512 路径 | 构建历史 vectorED driver；不要求 parasail |
| `make test_all_backends_small` | SSE + AVX2 + AVX512 | 小型可读数据集的一致性验证 |
| `make test_avx512_correctness` | AVX512 | AVX512 bit conversion、shift、mask、VtH 和 full pass/fail 测试 |

## 清理

```bash
make clean
```

会删除主要二进制、对象文件和测试生成的 runner/test binary。

## 当前测试状态说明

小型后端一致性测试：

```bash
make CXX=g++ test_all_backends_small
```

当前通过。

完整 AVX512 correctness 测试中的 `test_avx512_full_correctness` 可能暴露 LEAP global pass/fail 层面的算法一致性问题。该测试使用 scalar Levenshtein DP 作为测试内 oracle，不影响生产 LEAP-BV 实现。若该测试失败，应优先查看输出中的具体 read/reference、threshold 和 scalar distance。

最近一次验证中，以下 AVX512 primitive 测试通过：

```bash
make CXX=g++ test_avx512_bit_convert
make CXX=g++ test_avx512_shift
make CXX=g++ test_avx512_lane_masks
make CXX=g++ test_avx512_vth
```

`make CXX=g++ test_avx512_correctness` 目前在 `test_avx512_full_correctness` 处失败，复现输出为：

```text
test_avx512_full_correctness mismatch case=equal_length_global length=7 threshold=5 expected=0 actual=1 scalar_distance=6
```

## 开发注意事项

- 不要把 LEAP 生产路径替换成 DP、Myers、WFA、parasail 或直接 ASCII 比较。
- scalar DP 只允许出现在测试 oracle 中。
- 新增 SIMD 后端时，应保持原始数据流：

```text
DNA -> bit0/bit1 -> shift -> XOR/OR masks -> VtH -> LEAP recurrence
```

- SSE/AVX2/AVX512 的 active effective length 应分别保持为 128/256/512，不要因为全局 buffer 容量变大而让旧后端扫描更宽位数。
