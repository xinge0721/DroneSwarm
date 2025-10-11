import sys
import json
import os
from vosk import Model, KaldiRecognizer
import wave

def test_model(model_path, audio_file):
    """测试单个Vosk模型的功能性"""
    print(f"\n{'='*60}")
    print(f"测试模型: {os.path.basename(model_path)}")
    print(f"{'='*60}")
    
    try:
        # 尝试加载模型
        model = Model(model_path)
        print(f"✅ 模型加载成功")
        
        # 尝试打开音频文件
        wf = wave.open(audio_file, "rb")
        
        # 创建识别器
        rec = KaldiRecognizer(model, wf.getframerate())
        
        # 尝试读取一些音频数据
        data = wf.readframes(4000)
        if len(data) == 0:
            print("⚠️ 警告: 音频文件为空")
            return False
        
        # 尝试处理音频
        if rec.AcceptWaveform(data):
            result = json.loads(rec.Result())
            print("识别片段结果:", result.get('text', ''))
        
        # 尝试获取最终结果
        final_result = json.loads(rec.FinalResult())
        full_text = final_result.get('text', '')
        
        if full_text:
            print("\n识别结果:")
            print("-"*60)
            print(full_text)
            print("-"*60)
            print("✅ 功能测试通过: 模型成功处理音频并返回文本")
            return True
        else:
            print("⚠️ 警告: 模型未返回识别文本")
            return False
            
    except Exception as e:
        print(f"❌ 测试失败: {str(e)}")
        return False

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("用法: python3 test_vosk.py <模型目录> <音频文件>")
        sys.exit(1)
    
    model_dir = sys.argv[1]
    audio_file = sys.argv[2]
    
    # 测试所有模型
    success_count = 0
    total_count = 0
    
    print("测试策略: 验证模型加载和基本识别功能")
    print("注意: 文本结果可能因音频内容而异，主要验证功能完整性")
    
    for model_name in os.listdir(model_dir):
        model_path = os.path.join(model_dir, model_name)
        if os.path.isdir(model_path):
            # 检查是否是有效的Vosk模型
            if os.path.exists(os.path.join(model_path, "am/final.mdl")):
                total_count += 1
                if test_model(model_path, audio_file):
                    success_count += 1
            else:
                print(f"\n{'='*60}")
                print(f"跳过无效模型: {model_name}")
                print("原因: 缺少模型文件 (am/final.mdl 不存在)")
                print(f"{'='*60}")
    
    print("\n" + "="*60)
    print(f"功能测试总结: {success_count}/{total_count} 个模型功能正常")
    print("="*60)
    
    # 检查关键模型
    key_model = "vosk-model-cn-kaldi-multicn-0.15"
    key_model_path = os.path.join(model_dir, key_model)
    if os.path.exists(key_model_path) and os.path.exists(os.path.join(key_model_path, "am/final.mdl")):
        print(f"\n检查关键模型: {key_model}")
        if test_model(key_model_path, audio_file):
            print("✅ 关键模型功能正常")
        else:
            print("❌ 关键模型功能异常 - 需要手动检查")
    else:
        print(f"⚠️ 警告: 关键模型 {key_model} 未找到或无效")
