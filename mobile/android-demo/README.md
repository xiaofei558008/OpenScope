# OpenScope Android DEMO

进程内中文演示，不是桌面工程的合并，也不是可连接产品的客户端。

独立目录 `mobile/android-demo`。基准提交 `cc046627e3b9ed08253ca2ff54929a06ac3cd8c7` 只用来放这一个演示，不表示接受 `project_tab` `d67c139`。桌面源码没有改动。

## 目录检查

在该提交上，仓库根是桌面 OpenScope（`code/`、`module/`、`dll/`、`tests/`、`packaging/`、`assets/`、`icon/`）和 BMAD 工具。没有 `mobile/`、`android/`、Gradle 或 Kotlin 工程。名称里带 mobile 的只有 BMAD UX 示例文档。`mobile/android-demo` 原先不存在，因此用作这个隔离演示。以后若有正式 App，可以跟这个目录并存。

## 演示里有什么

界面持久显示 **DEMO · 模拟数据**。手机单栏，宽屏（约 840dp 起）用侧栏。

- 三种模式名称：局域网直连 PC、官方托管云、自托管。全部在进程内模拟。局域网失败不会改成官方云，证书失败不会变成明文。
- 模拟扫码和手输。不申请相机。声明和验证结果分开，申请要明确按下，然后单独等待 PC 同意。通过后默认是观察者。控制不可执行。
- 可模拟拒绝、取消、超时、过期、吊销。记住的设备只是点选项，不是持久授权。删除本地记录不会变成服务端吊销。
- 同一条已验证申请再读一次，只返回当前审批（包括已拒绝），不二次兑换、不新发授权、不延长有效期。这些是演示语义，不是桌面 ABI，也没有冻结 TTL 或二维码格式。
- 命令保留七个原始状态：`received`、`accepted`、`running`、`succeeded`、`failed`、`rejected`、`unknown`。界面分成五组，进行中保留子文案。本地把握单独显示。提交后断线把本地把握标成未知，不改原始状态，不重放，没有 `control_query`。
- 工程 / 模块 / 变量、数值和有界合成波形。波形可冻结视窗和缩放。冻结不停掉生成。换代数会清掉旧样本并写出来源变化。后台暂停生成，回到前台先复核再在曲线上留缺口。
- 订单和权益是占位。支付回跳不会把权益写成已开通。没有价格、结账或支付 SDK。

演示节拍写在 `DemoOnlySettings`：环容量 240、最多 4 条曲线、生成 100 ms、界面刷新 250 ms。这是演示夹具，不是产品性能指标或配额。

## 明确不做

不连接产品端点，不开监听，不扫真实二维码，不申请运行时权限，不用云、控制或支付 SDK，不对设备执行 shell、控制或烧写。清单里没有 `INTERNET` 和 `CAMERA`。

## 构建

需要 Android SDK（已用 compileSdk 35、build-tools 35.0.0 编过 debug 包）。在本目录写 `local.properties`：

```properties
sdk.dir=/path/to/android-sdk
```

然后：

```bash
./gradlew :app:testDebugUnitTest :app:lintDebug :app:assembleDebug
```

Debug APK：

```text
app/build/outputs/apk/debug/app-debug.apk
```

这是 debug 包，由 Android Gradle 插件使用本机 debug 证书签名，便于安装。仓库里没有 keystore，也没有 release 签名配置。不要把这个 APK 当作公开发布包。

应用 id：`app.openscope.androiddemo`。启动后横幅应一直可见。模拟场景可以直接点 `demo-ok`，再按「验证来源」「申请配对」「模拟 PC 同意」。官方托管云还要把「服务端权益 active」点上，观察才会开始；局域网直连不看这个占位权益。

## 限制

- 没有真实 PC、云、相机或支付。按钮上的「模拟」只改本进程状态。
- 错误码和场景编号是演示标签，协议未冻结，不能拿去对接桌面或云端。
- 波形数字是合成的。界面上的节拍说明不是实测性能。
- 单元测试跑在开发机 JVM 上。是否在模拟器或真机里点过，以当次交付说明为准，不要把测试数量当成验收。
