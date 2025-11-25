document.addEventListener('DOMContentLoaded', () => {
  const backdrop = document.getElementById('start-backdrop');
  const confirmBtn = document.getElementById('confirm-btn');
  const popupLayer = document.getElementById('popup-layer');
  const balls = document.getElementById('float-balls');
  const music = document.getElementById('bgMusic');

  const messages = [
    '自己是最棒的', '好好努力', '你超棒的', '加油！', '今天也要开心', '记得锻炼', '不要熬夜', '早点休息', '天天开心', '保持热爱'
  ];
  const themes = [
    'theme-blue', 'theme-green', 'theme-orange', 'theme-purple', 'theme-pink', 'theme-yellow', 'theme-cyan', 'theme-lime', 'theme-red', 'theme-teal', 'theme-indigo', 'theme-amber', 'theme-rose', 'theme-mint', 'theme-peach', 'theme-lavender', 'theme-coral', 'theme-sky', 'theme-lemon'
  ];
  const anims = ['anim-top', 'anim-bottom', 'anim-left', 'anim-right', 'anim-topleft', 'anim-topright', 'anim-bottomleft', 'anim-bottomright'];

  let started = false;
  let timer = null;
  let count = 0;
  const maxPopups = 100;
  const intervalMs = 500;

  function randPick(arr) { return arr[Math.floor(Math.random() * arr.length)]; }
  function randInt(min, max) { return Math.floor(Math.random() * (max - min + 1)) + min; }

  function makePopup() {
    if (count >= maxPopups) return;
    const popup = document.createElement('div');
    popup.className = `popup ${randPick(themes)} ${randPick(anims)}`;

    const header = document.createElement('div');
    header.className = 'header';
    const icon = document.createElement('span');
    icon.className = 'icon';
    icon.textContent = '💝';
    const title = document.createElement('span');
    title.className = 'title';
    title.textContent = '提示';
    header.appendChild(icon);
    header.appendChild(title);

    const content = document.createElement('div');
    content.className = 'content';
    content.textContent = randPick(messages);

    popup.appendChild(header);
    popup.appendChild(content);

    const vw = window.innerWidth;
    const vh = window.innerHeight;
    const left = randInt(5, Math.max(5, vw - 230 - 10));
    const top = randInt(5, Math.max(5, vh - 100 - 10));
    const rotate = randInt(-5, 5);

    popup.style.left = `${left}px`;
    popup.style.top = `${top}px`;
    popup.style.transform = `rotate(${rotate}deg)`;
    popup.style.opacity = String(Math.max(0.3, 1 - count / 100));

    popupLayer.appendChild(popup);
    count++;
  }

  function start() {
    if (started) return;
    started = true;
    backdrop.setAttribute('aria-hidden', 'true');
    backdrop.style.display = 'none';
    try { music.volume = 0.6; music.loop = true; music.play().catch(() => { }); } catch (e) { }
    balls.style.display = 'flex';
    balls.classList.add('show');
    timer = setInterval(() => {
      makePopup();
      if (count >= maxPopups) { clearInterval(timer); timer = null; }
    }, intervalMs);
  }

  confirmBtn.addEventListener('click', start);
  document.addEventListener('keydown', (e) => { if (e.key === 'Enter') start(); });
});

