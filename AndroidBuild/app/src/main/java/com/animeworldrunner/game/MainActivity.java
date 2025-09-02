package com.animeworldrunner.game;

import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        WebView webView = new WebView(this);
        webView.setWebViewClient(new WebViewClient());
        webView.getSettings().setJavaScriptEnabled(true);
        
        // Load a simple anime-style game demo
        String gameHtml = "<!DOCTYPE html>" +
            "<html><head><title>Anime World Runner</title>" +
            "<style>body{margin:0;background:linear-gradient(45deg,#ff6b9d,#c44569);font-family:Arial;color:white;text-align:center;padding:50px;}" +
            ".title{font-size:32px;margin:20px;text-shadow:2px 2px 4px rgba(0,0,0,0.5);}" +
            ".character{font-size:100px;animation:bounce 2s infinite;}" +
            "@keyframes bounce{0%,100%{transform:translateY(0);} 50%{transform:translateY(-20px);}}" +
            ".controls{margin:30px;font-size:18px;}" +
            ".button{background:#ff4757;border:none;color:white;padding:15px 30px;margin:10px;border-radius:25px;font-size:16px;}" +
            "</style></head><body>" +
            "<div class='title'>🎌 Anime World Runner</div>" +
            "<div class='character'>🏃‍♀️</div>" +
            "<div class='controls'>Genshin Impact Style Runner</div>" +
            "<div>✨ 3D Movement & Abilities</div>" +
            "<div>🎮 Touch Controls Ready</div>" +
            "<div>🌸 Character Customization</div>" +
            "<div>🏔️ Procedural Environments</div>" +
            "<br><button class='button' onclick='alert(\"Game Starting Soon!\")'>Start Adventure</button>" +
            "<button class='button' onclick='alert(\"Customization Menu\")'>Customize</button>" +
            "</body></html>";
        
        webView.loadData(gameHtml, "text/html", "UTF-8");
        setContentView(webView);
    }
}
