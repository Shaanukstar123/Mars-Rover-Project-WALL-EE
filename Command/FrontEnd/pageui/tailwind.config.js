/** @type {import('tailwindcss').Config} */

module.exports = {
  content: [
    "./src/**/*.{js,jsx,ts,tsx}",
  ],
  theme: {
    extend: {
      borderRadius: {
        '4xl': '1.5rem',
      },
      colors: {
        'rblue': '#243c5a',
        'apple': '#f5f5f7'
      }
    },
  },
  plugins: [],
}
