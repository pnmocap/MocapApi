Shader "UI/RoundUIMaskWithColor"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {} // 主纹理
        _Color ("Color Tint", Color) = (1,1,1,1) // 动态颜色
        _Radius ("Circle Radius", Range(0, 1)) = 0.5 // 圆形裁剪半径
        _Center ("Circle Center", Vector) = (0.5, 0.5, 0, 0) // 圆形中心点
        _EdgeSoftness ("Edge Softness", Range(0, 0.1)) = 0.02 // 边缘平滑度
    }
    SubShader
    {
        Tags { "Queue"="Overlay" "IgnoreProjector"="True" "RenderType"="Transparent" }
        Blend SrcAlpha OneMinusSrcAlpha // 透明混合模式
        ZWrite Off                     // 禁止写入深度缓冲
        LOD 100

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"

            struct appdata_t
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                float4 vertex : SV_POSITION;
            };

            sampler2D _MainTex;
            fixed4 _Color;           // 动态颜色
            float _Radius;           // 圆形裁剪半径
            float4 _Center;          // 圆形中心点
            float _EdgeSoftness;     // 边缘平滑度

            v2f vert (appdata_t v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                float2 uv = i.uv;

                // 计算像素与圆心的距离
                float2 center = _Center.xy;
                float dist = length(uv - center);

                // 使用 smoothstep 计算透明边缘
                float alpha = 1.0 - smoothstep(_Radius - _EdgeSoftness, _Radius, dist);

                // 如果在裁剪区域之外，直接返回透明
                if (alpha <= 0.0)
                {
                    return fixed4(0, 0, 0, 0); // 完全透明
                }

                // 从主纹理采样颜色并叠加动态颜色
                fixed4 texColor = tex2D(_MainTex, uv);
                return texColor * _Color * alpha;
            }
            ENDCG
        }
    }
    FallBack "UI/Default"
}
