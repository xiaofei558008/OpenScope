# OpenScope 发布指南（www.opendebugger.com 软件下载）

OpenScope 安装包发布到网站 `www.opendebugger.com/downloads/`，全程自动化。

## 一、一键发布（推荐）

打包 + 发布一步完成：

```bat
python packaging\make_setup.py --publish
```

该命令会：
1. 从 `code/src/version.rc` 读取版本号，用 Inno Setup 打包 → `dist\OpenScope-Setup-<版本>.exe`
2. 自动调用 `publish.py`：上传安装包 + 重新生成下载页 + HTTPS 自检

## 二、仅重新发布现有安装包

```bat
python publish.py            # 上传新增/更新的安装包 + 刷新下载页
python publish.py --all      # 强制上传所有安装包
python publish.py --dry-run  # 预演，不实际上传
```

## 三、发布内容

| 项 | 说明 |
|----|------|
| 下载页 | `https://www.opendebugger.com/downloads/` |
| 安装包目录 | 服务器 `/var/www/downloads/openscope/` |
| 最新版 | 自动置顶并标注"最新版"徽标 |
| 校验和 | 每版自动计算 SHA256 并展示在下载页 |

## 四、新增版本的完整流程（惯例）

1. 开发 → 构建：`python build.py --quiet`（0 error / 0 warning）
2. 修改 `code/src/version.rc` 版本号（唯一版本来源），同步 `packaging/openscope.iss`
3. 回归测试 + 打包：`python packaging\make_setup.py --publish`（自动发布）
4. 提交 git + 打 tag + 推送双远端（gitee/github）：`git tag v1.10.0`，见 PROGRESS.md 惯例

## 五、服务器端结构

```
/var/www/downloads/
├── index.html            # 自动生成的下载页（publish.py 每次覆盖）
└── openscope/
    └── OpenScope-Setup-1.10.0.exe   # 各版本安装包
```

nginx：`location ^~ /downloads/` → alias `/var/www/downloads/`，.exe 返回
`Content-Disposition: attachment; filename="OpenScope-Setup-1.10.0.exe"`（**必须带 filename=**，
否则部分浏览器会把下载文件保存为链接文字"下载"）+ `application/x-msdownload`，支持断点续传（Accept-Ranges）。
> 注意：嵌套 location 的匹配正则要能覆盖子目录（`/downloads/openscope/...`），用
> `([^/]+\.(exe|zip|...))$` 捕获文件名即可。

## 六、自检

`publish.py` 每次运行结束自动：
- 校验下载页 `HTTP 200`
- 校验最新安装包 `HTTP 200` 且字节数与本地一致
- 可手工核验 SHA256：下载页每行展示，或本地 `python -c "import hashlib;print(hashlib.sha256(open(r'dist\OpenScope-Setup-1.10.0.exe','rb').read()).hexdigest())"`
