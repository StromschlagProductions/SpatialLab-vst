const { chromium } = require('playwright');
const fs = require('fs');
(async()=>{
 const browser=await chromium.launch({executablePath:process.env.CHROMIUM_PATH||'/usr/bin/chromium',headless:true,args:['--no-sandbox','--autoplay-policy=no-user-gesture-required']});
 const page=await browser.newPage();const errors=[];page.on('pageerror',e=>errors.push(e.message));
 await page.goto(process.env.DEMO_URL||require('url').pathToFileURL(require('path').join(process.argv[2],'index.html')).href);
 const count=await page.locator('section button').count();if(count!==14)throw Error('Expected 14 comparison buttons');
 for(let i=0;i<count;i++){
  const button=page.locator('section button').nth(i);await button.click();
  await page.waitForFunction(()=>[...document.querySelectorAll('audio')].some(a=>!a.paused&&a.readyState>=2),null,{timeout:15000});
  const state=await page.evaluate(()=>[...document.querySelectorAll('audio')].filter(a=>!a.paused).map(a=>({src:a.currentSrc,duration:a.duration,volume:a.volume,error:a.error?.code})));
  if(state.length!==1||!state[0].duration||state[0].volume!==1||state[0].error)throw Error(JSON.stringify(state));
 }
 await page.locator('section button').nth(0).click();await page.waitForFunction(()=>document.querySelector('audio').readyState>=2);
 await page.evaluate(()=>document.querySelector('audio').currentTime=2);
 await page.locator('section button').nth(1).click();await page.waitForFunction(()=>{const a=document.querySelector('audio');return a.readyState>=2&&a.currentTime>=1.95});
 const images=await page.locator('img').evaluateAll(imgs=>imgs.every(i=>i.complete&&i.naturalWidth>0));if(!images||errors.length)throw Error(JSON.stringify(errors));
 await page.screenshot({path:process.argv[2]+'/Browser-Test.png',fullPage:true});
 const result={browser:'Chromium',mode:process.env.DEMO_URL||'file:// offline',audioButtons:count,allTracksDecodeAndPlay:true,onlyOnePlaying:true,volume:1,positionSwitch:true,screenshotsLoaded:true,pageErrors:errors};
 fs.writeFileSync(process.argv[2]+'/browser-test.json',JSON.stringify(result,null,2));console.log(result);await browser.close();
})().catch(e=>{console.error(e);process.exit(1)});
