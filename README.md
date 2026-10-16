# tiej

tie 桌面（JIT）；可单独使用。

状态：**执行域引擎已落地** —— `src/core/execrun.tie`（tiej 执行域：命令行执行引擎——  
直启 / 直通 / 捕获 / 进程管道 / 重定向 / 复合链；Windows 与 POSIX 单源双实现，  
`#cfg` 按目标裁剪）。tshell 经 `import` 消费（tshell 绑定 tiej 的第一步）。  
JIT / 内嵌 LLVM 后续批次。

## 引擎（execrun）

- **直启**：CreateProcessW / fork+execv 直接起进程（无 shell 借用）；  
  `cwd` 经 `lpCurrentDirectory` / `chdir` 直传——不拼接 `cd … && …` 前缀串。
- **无 shell 借用**：`&&` / `||` / `;` / 进程管道 / 重定向全部引擎内实现，  
  rc 驱动、逐段可读；启动失败 `started=false` + 明确诊断（不静默）。
- **接口**：单段 `run_one(Seg)`；便捷 `run` / `capture` / `redir`；  
  多段 `run_pipeline` / `chain` / `pipe2`（段与结果跨界用编码串——  
  p.9.12.6 边界：`table<R>`（R 为 struct）不能作函数形参/返回值）。
- **平台**：Windows（CreateProcessW 直启）已全量实测（26 项探针全绿）；
  POSIX（fork+chdir+dup2+execv，pipe2/O_CLOEXEC fd 卫生）已随同批落地，
  本机无 Linux 运行时：验证止于「前端+IR 全过、链接仅缺本机 Linux CRT」层级。
- **已知限制**（登记）：双流捕获顺序读、投喂顺序写——极端双饱和场景存在阻塞  
  风险，后续批次以并发治理。

测试：从本仓根运行（产物统一 `_local_only/bin/`，不进源码树）

```
mkdir -p _local_only/bin
tiec tests/exec_probe.tie -o _local_only/bin/exec_probe.exe && _local_only/bin/exec_probe.exe
tiec tests/fsglob_probe.tie -o _local_only/bin/fsglob_probe.exe && _local_only/bin/fsglob_probe.exe
tiec tests/term_probe.tie -o _local_only/bin/term_probe.exe && _local_only/bin/term_probe.exe
```

（exec 26 项：PATH/rc/cwd/中文 UTF-8 往返/管道/重定向/合流/复合链/跳过传播；
fsglob 10 项；term 4 项（非 TTY 降级面）。）

POSIX 运行级验证（CI，ubuntu-latest）：workflow `linux-verify` 取回
`ci-linux-ir` release 资产（本机最新 stage0 tiec 生成的 Linux 目标 IR，
刷新：`tiec compiler/driver.tie --target linux-x64 --emit-ir -o driver-linux`
后 `gh release upload ci-linux-ir driver-linux.ll --clobber`），经零 Win32
行为门禁与 clang/lld 链接出 tiec-linux，编译并运行三个探针并断言
26/10/4 全绿。已知账（垫片在 `.github/ci/linux-shims.c`）：内嵌解释器
dynload 缺 linux 分支；控制台契约 linux 分支应发 `write` 而非 `_write`。

