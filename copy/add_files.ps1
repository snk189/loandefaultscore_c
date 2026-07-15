Write-Host "Cleaning up the done folders..." -ForegroundColor Cyan
Remove-Item -Path "pdfs\done\*\*" -Force -Recurse -ErrorAction SilentlyContinue

Write-Host "Restoring pending PDFs from backup..." -ForegroundColor Cyan
Copy-Item -Path "copy\*.pdf" -Destination "pdfs\pending" -Force

Write-Host "Done! The pending folder is fully restocked and ready for your demo!" -ForegroundColor Green
