@echo off
REM 在当前路径下执行 Git 命令

REM 显示当前路径
echo 当前路径: %cd%

REM 执行 git add
git add .

REM 执行 git commit
git commit -m "update"

REM 执行 git push
git push -u origin master

REM 提示完成
echo 上传完成！
pause
