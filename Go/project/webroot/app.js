// 初始化场景、相机和渲染器
const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
const renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });

renderer.setSize(window.innerWidth, window.innerHeight);
renderer.setPixelRatio(window.devicePixelRatio);
// 开启色调映射，让亮部更自然
renderer.toneMapping = THREE.ReinhardToneMapping;
document.getElementById('canvas-container').appendChild(renderer.domElement);

// 轨道控制器
const controls = new THREE.OrbitControls(camera, renderer.domElement);
controls.enableDamping = true;
controls.dampingFactor = 0.05;
controls.enableZoom = true;
controls.autoRotate = true;
controls.autoRotateSpeed = 0.5;

// 相机初始位置
camera.position.set(0, 2, 6);
camera.lookAt(0, 0, 0);

// --- 后期处理 (Bloom 辉光) ---
const renderScene = new THREE.RenderPass(scene, camera);

// 分辨率, 强度, 半径, 阈值
const bloomPass = new THREE.UnrealBloomPass(
    new THREE.Vector2(window.innerWidth, window.innerHeight),
    2.0,  // 强度 strength
    0.4,  // 半径 radius
    0.1   // 阈值 threshold (只对亮于此的部分发光)
);

const composer = new THREE.EffectComposer(renderer);
composer.addPass(renderScene);
composer.addPass(bloomPass);

// --- 核心黑洞 (事件视界) ---
// 使用黑色材质，但为了在辉光中显出轮廓，可以稍微加一点点极暗的蓝色
const blackHoleGeometry = new THREE.SphereGeometry(1.0, 64, 64);
const blackHoleMaterial = new THREE.MeshBasicMaterial({ color: 0x000000 });
const blackHole = new THREE.Mesh(blackHoleGeometry, blackHoleMaterial);
scene.add(blackHole);

// --- 科技感吸积盘 Shader ---
// 采用青色/紫色调，增加扫描线效果
const diskVertexShader = `
varying vec2 vUv;
varying vec3 vPos;
void main() {
    vUv = uv;
    vPos = position;
    gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
}
`;

const diskFragmentShader = `
uniform float iTime;
varying vec2 vUv;

// 噪声函数
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main() {
    vec2 centered = vUv - 0.5;
    float r = length(centered) * 2.0; 
    float angle = atan(centered.y, centered.x);

    if (r < 0.2 || r > 1.0) discard;

    // 动态旋转
    float speed = 3.0 / (r * r + 0.1);
    float rotAngle = angle + iTime * speed;

    // 基础噪声纹理 (模拟气体流)
    float n1 = noise(vec2(r * 8.0, rotAngle * 4.0));
    float n2 = noise(vec2(r * 15.0, rotAngle * 8.0 - iTime));
    
    // 扫描线/网格效果 (科技感)
    float grid = abs(sin(r * 50.0 - iTime * 2.0));
    grid = smoothstep(0.0, 0.8, grid);
    
    // 颜色定义 (青色 -> 紫色)
    vec3 cyan = vec3(0.0, 1.0, 1.0);
    vec3 purple = vec3(0.8, 0.0, 1.0);
    vec3 deepBlue = vec3(0.0, 0.1, 0.5);

    // 混合颜色
    vec3 color = mix(deepBlue, cyan, n1);
    color = mix(color, purple, n2 * grid);

    // 高亮脉冲
    float pulse = sin(iTime * 2.0) * 0.5 + 0.5;
    color += vec3(0.2) * pulse * grid;

    // 边缘柔和与发光增强
    float alpha = smoothstep(0.2, 0.3, r) * smoothstep(1.0, 0.6, r);
    
    // 亮度倍增 (配合 Bloom)
    color *= 3.0; 

    gl_FragColor = vec4(color, alpha);
}
`;

const diskGeometry = new THREE.RingGeometry(1.05, 3.8, 128, 1);
// UV 映射调整
const pos = diskGeometry.attributes.position;
const v3 = new THREE.Vector3();
for (let i = 0; i < pos.count; i++){
    v3.fromBufferAttribute(pos, i);
    diskGeometry.attributes.uv.setXY(i, v3.x/8.0 + 0.5, v3.y/8.0 + 0.5);
}

const diskUniforms = {
    iTime: { value: 0 }
};

const diskMaterial = new THREE.ShaderMaterial({
    vertexShader: diskVertexShader,
    fragmentShader: diskFragmentShader,
    uniforms: diskUniforms,
    side: THREE.DoubleSide,
    transparent: true,
    depthWrite: false,
    blending: THREE.AdditiveBlending
});

const accretionDisk = new THREE.Mesh(diskGeometry, diskMaterial);
accretionDisk.rotation.x = -Math.PI / 2;
scene.add(accretionDisk);


// --- 粒子环 (Quantum Particles) ---
const particleCount = 1000;
const particleGeometry = new THREE.BufferGeometry();
const particlePositions = new Float32Array(particleCount * 3);
const particleSizes = new Float32Array(particleCount);

for(let i = 0; i < particleCount; i++) {
    // 随机分布在圆环区域
    const theta = Math.random() * Math.PI * 2;
    const radius = 2.5 + Math.random() * 2.0; // 稍微靠外
    const x = Math.cos(theta) * radius;
    const z = Math.sin(theta) * radius;
    const y = (Math.random() - 0.5) * 0.2; // 稍微有点厚度

    particlePositions[i * 3] = x;
    particlePositions[i * 3 + 1] = y;
    particlePositions[i * 3 + 2] = z;

    particleSizes[i] = Math.random();
}

particleGeometry.setAttribute('position', new THREE.BufferAttribute(particlePositions, 3));
particleGeometry.setAttribute('size', new THREE.BufferAttribute(particleSizes, 1));

const particleMaterial = new THREE.PointsMaterial({
    color: 0x00ffff,
    size: 0.05,
    transparent: true,
    opacity: 0.8,
    blending: THREE.AdditiveBlending
});

const particleRing = new THREE.Points(particleGeometry, particleMaterial);
scene.add(particleRing);


// --- 星空背景 (深邃版) ---
const starGeo = new THREE.BufferGeometry();
const starCnt = 3000;
const starPos = new Float32Array(starCnt * 3);
for(let i=0; i<starCnt*3; i++) {
    starPos[i] = (Math.random() - 0.5) * 200;
}
starGeo.setAttribute('position', new THREE.BufferAttribute(starPos, 3));
const starMat = new THREE.PointsMaterial({ color: 0x8888aa, size: 0.1, transparent: true, opacity: 0.5 });
const stars = new THREE.Points(starGeo, starMat);
scene.add(stars);


// --- 动画循环 ---
const clock = new THREE.Clock();

function animate() {
    requestAnimationFrame(animate);

    const elapsedTime = clock.getElapsedTime();

    // 更新 Shader 时间
    diskUniforms.iTime.value = elapsedTime;

    // 旋转粒子环
    particleRing.rotation.y = -elapsedTime * 0.1;

    // 缓慢旋转背景
    stars.rotation.y = elapsedTime * 0.02;

    controls.update();
    
    // 使用 composer 替代 renderer.render
    composer.render();
}

window.addEventListener('resize', () => {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
    composer.setSize(window.innerWidth, window.innerHeight);
});

animate();