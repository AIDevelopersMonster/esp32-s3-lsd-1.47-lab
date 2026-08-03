# Publish the prepared project to GitHub

Recommended repository name:

```text
esp32-s3-lcd-1.47-lab
```

Create an empty public repository without adding a README, license or
`.gitignore`, because those files are already included here.

From PowerShell in the unzipped project directory:

```powershell
git init
git add .
git commit -m "Initial ESP32-S3-LCD-1.47 lab project"
git branch -M main
git remote add origin https://github.com/AIDevelopersMonster/esp32-s3-lcd-1.47-lab.git
git push -u origin main
```

After the first push, open the **Actions** tab. The `Build firmware` workflow
will install PlatformIO and compile the main firmware.
