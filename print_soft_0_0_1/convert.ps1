# ************************ 配置项（需修改为你的实际路径）************************
$projectPath = "D:\work_namespace\Code\print_moudle\manu_print_soft\print_soft_0_0_1" # 项目根路径
$targetExtensions = @("*.h", "*.cpp") # 目标文件类型：.h 和 .cpp
$sourceEncoding = "GB2312" # 源文件编码：GB2312
$targetEncoding = "UTF8" # 目标编码：UTF-8（带BOM，PowerShell的UTF8默认带BOM）
# ********************************************************************************

# 递归遍历所有目标文件并转换
foreach ($ext in $targetExtensions) {
    # 递归查找所有对应扩展名的文件
    $files = Get-ChildItem -Path $projectPath -Recurse -Filter $ext -ErrorAction SilentlyContinue
    if ($files.Count -eq 0) {
        Write-Host "未找到 $ext 类型文件"
        continue
    }

    # 逐个转换文件
    foreach ($file in $files) {
        try {
            # 读取源文件（以GB2312编码打开，避免读取乱码）
            $content = Get-Content -Path $file.FullName -Encoding $sourceEncoding -Raw -ErrorAction Stop
            # 保存文件（以UTF-8带BOM编码写入）
            $content | Out-File -Path $file.FullName -Encoding $targetEncoding -Force -ErrorAction Stop
            Write-Host "✅ 转换成功：$($file.FullName)"
        }
        catch {
            Write-Warning "❌ 转换失败：$($file.FullName)，错误信息：$($_.Exception.Message)"
        }
    }
}

Write-Host "`n所有文件转换流程结束！"
Pause # 暂停窗口，方便查看转换结果