# Publish the prepared project to GitHub

Current repository:

```text
AIDevelopersMonster/lab-esp32-s3-lcd-1.47
```

If a local clone still points to the previous repository name, update `origin` with:

```powershell
git remote set-url origin https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47.git
git remote -v
```

For a fresh local checkout from an unzipped project directory:

```powershell
git init
git add .
git commit -m "Initial ESP32-S3-LCD-1.47 lab project"
git branch -M main
git remote add origin https://github.com/AIDevelopersMonster/lab-esp32-s3-lcd-1.47.git
git push -u origin main
```

After the first push, open the **Actions** tab. The `Build firmware` workflow
will install PlatformIO and compile the main firmware.
