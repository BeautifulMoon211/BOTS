#!/usr/bin/env python3
"""
Generate a simple icon for LinkCopier extension
Requires: pip install pillow
"""

try:
    from PIL import Image, ImageDraw, ImageFont
    import os
    
    # Create a 128x128 image with blue background
    size = 128
    img = Image.new('RGB', (size, size), color='#4285f4')
    draw = ImageDraw.Draw(img)
    
    # Draw a chain link icon
    # Left circle
    draw.ellipse([25, 44, 55, 74], outline='white', width=6)
    # Right circle  
    draw.ellipse([73, 44, 103, 74], outline='white', width=6)
    # Connecting lines
    draw.line([40, 44, 88, 44], fill='white', width=6)
    draw.line([40, 74, 88, 74], fill='white', width=6)
    
    # Add F9 text
    try:
        font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 20)
    except:
        font = ImageFont.load_default()
    
    text = "F9"
    # Get text bounding box for centering
    bbox = draw.textbbox((0, 0), text, font=font)
    text_width = bbox[2] - bbox[0]
    text_height = bbox[3] - bbox[1]
    text_x = (size - text_width) // 2
    text_y = 95
    
    draw.text((text_x, text_y), text, fill='white', font=font)
    
    # Save the image
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_path = os.path.join(script_dir, 'icon.png')
    img.save(output_path)
    print(f"Icon saved to: {output_path}")
    
except ImportError:
    print("PIL/Pillow not installed. Install with: pip install pillow")
    print("Alternatively, open create-icon.html in a browser and save the canvas as icon.png")

