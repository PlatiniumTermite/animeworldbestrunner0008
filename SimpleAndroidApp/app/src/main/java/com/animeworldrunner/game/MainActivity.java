package com.animeworldrunner.game;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.graphics.Color;
import android.view.Gravity;
import android.widget.Toast;

public class MainActivity extends Activity {
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // Create main layout
        LinearLayout mainLayout = new LinearLayout(this);
        mainLayout.setOrientation(LinearLayout.VERTICAL);
        mainLayout.setBackgroundColor(Color.parseColor("#FF6B9D"));
        mainLayout.setGravity(Gravity.CENTER);
        mainLayout.setPadding(50, 50, 50, 50);
        
        // Title
        TextView title = new TextView(this);
        title.setText("🎌 Anime World Runner");
        title.setTextSize(32);
        title.setTextColor(Color.WHITE);
        title.setGravity(Gravity.CENTER);
        title.setPadding(20, 20, 20, 40);
        
        // Character emoji
        TextView character = new TextView(this);
        character.setText("🏃‍♀️");
        character.setTextSize(80);
        character.setGravity(Gravity.CENTER);
        character.setPadding(20, 20, 20, 20);
        
        // Description
        TextView desc = new TextView(this);
        desc.setText("Genshin Impact Style Runner\n\n✨ 3D Movement & Abilities\n🎮 Touch Controls\n🌸 Character Customization\n🏔️ Procedural Environments");
        desc.setTextSize(16);
        desc.setTextColor(Color.WHITE);
        desc.setGravity(Gravity.CENTER);
        desc.setPadding(20, 20, 20, 40);
        
        // Start button
        Button startButton = new Button(this);
        startButton.setText("Start Adventure");
        startButton.setBackgroundColor(Color.parseColor("#FF4757"));
        startButton.setTextColor(Color.WHITE);
        startButton.setTextSize(18);
        startButton.setPadding(40, 20, 40, 20);
        startButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(MainActivity.this, "🎮 Game Starting! Full 3D version coming soon!", Toast.LENGTH_LONG).show();
            }
        });
        
        // Customize button
        Button customizeButton = new Button(this);
        customizeButton.setText("Customize Character");
        customizeButton.setBackgroundColor(Color.parseColor("#5F27CD"));
        customizeButton.setTextColor(Color.WHITE);
        customizeButton.setTextSize(18);
        customizeButton.setPadding(40, 20, 40, 20);
        customizeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(MainActivity.this, "🎨 Character customization ready!", Toast.LENGTH_SHORT).show();
            }
        });
        
        // Add views to layout
        mainLayout.addView(title);
        mainLayout.addView(character);
        mainLayout.addView(desc);
        mainLayout.addView(startButton);
        
        LinearLayout.LayoutParams buttonParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.WRAP_CONTENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        );
        buttonParams.setMargins(0, 20, 0, 0);
        customizeButton.setLayoutParams(buttonParams);
        mainLayout.addView(customizeButton);
        
        setContentView(mainLayout);
    }
}
