import { defineConfig } from 'vite'

export default defineConfig({
    server: {
        port: 3000,
    },
    build: {
        assetsDir: '',
        rollupOptions: {
            output: {
                entryFileNames: 'index.js',
                chunkFileNames: '[name].js',
                assetFileNames: '[name][extname]',
            },
        },
    },
})
