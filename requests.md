# 1. Browser-style (HTML-first, XML fallback, then anything)
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8

# 2. API-only (JSON first, then plain text)
Accept: application/json;q=1.0,text/plain;q=0.5

# 3. Plain text or any
Accept: text/plain,*/*;q=0.1

# 4. XML or JSON equally
Accept: application/xml,application/json

# 5. Images (WebP first, then JPEG/PNG)
Accept: image/webp,image/apng,image/*;q=0.8,*/*;q=0.5

# 6. CSS and JavaScript
Accept: text/css,*/*;q=0.1
Accept: application/javascript;q=0.9,text/javascript;q=0.8,*/*;q=0.1

# 7. Binary download (any type)
Accept: */*

# 8. JSON with optional gzip encoding
Accept: application/json
Accept-Encoding: gzip, deflate, br

# 9. Locale-specific HTML
Accept: text/html;q=1.0,text/html;level=1;q=0.8

# 10. Custom vendor media types
Accept: application/vnd.myapp.v2+json,application/json;q=0.5
