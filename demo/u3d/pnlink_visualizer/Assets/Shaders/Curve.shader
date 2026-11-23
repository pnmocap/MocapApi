Shader "UI/Curve" {
    Properties {
        _MainTex ("Base (RGB)", 2D) = "white" {}
        _Blur ("Blur Radius", Range(0, 5)) = 1
    }

    SubShader {
        Tags { "Queue"="Transparent" "RenderType"="Transparent" }
        
        Pass {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #pragma require 2D
            #pragma exclude_renderers d3d11_9x // 禁用不必要的3D特性
            #pragma multi_compile _ PIXEL_SNAP_ON
            #if UNITY_IOS || UNITY_ANDROID
            precision mediump float; // 移动端优化
            #endif
            
            #include "UnityCG.cginc"

            struct v2f {
                float4 pos : SV_POSITION;
                float2 uv : TEXCOORD0;
            };

            sampler2D _MainTex;
            float4 _MainTex_TexelSize;
            float _Blur;

            v2f vert (appdata_base v) {
                v2f o;
                o.pos = UnityObjectToClipPos(v.vertex);
                
                // 2D专用：像素对齐优化
                #ifdef PIXEL_SNAP_ON
                o.pos = UnityPixelSnap(o.pos);
                #endif
                
                o.uv = v.texcoord;
                return o;
            }

            fixed4 frag (v2f i) : SV_Target {
                // 动态模糊采样
                float4 col = 0;
                for(int x=-2; x<=2; x++) {
                    for(int y=-2; y<=2; y++) {
                        float2 offset = float2(x,y) * _Blur * _MainTex_TexelSize.xy;
                        col += tex2D(_MainTex, i.uv + offset);
                    }
                }
                return col / 25.0;
            }
            ENDCG
        }
    }
}