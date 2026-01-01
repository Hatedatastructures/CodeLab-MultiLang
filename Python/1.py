import torch

print(torch.cuda.is_available())

# import tensorflow as tf
# print("TensorFlow版本:", tf.__version__)
# gpus = tf.config.list_physical_devices('GPU')
# if gpus:
#     print(f"✅ 检测到 {len(gpus)} 块GPU")
#     for gpu in gpus:
#         print(f" - {gpu}")
# else:
#     print("❌ 未检测到GPU，请检查驱动、CUDA、cuDNN及TensorFlow安装。")
