import {
  TIME_MAP,
  FALLBACK_IMAGE,
  IMAGE_BASE_PATH,
} from "../config/timeMapConfig.js";

function testTimeBasedImageSelection() {
  console.log("🧪 Starting Time-Based Image Selection Tests\n");

  const testCases = [
    { hour: 0, expectedFile: "12am1am.png", description: "Midnight (0:00)" },
    {
      hour: 1,
      expectedFile: "1am6am.png",
      description: "Early morning (1:00)",
    },
    { hour: 6, expectedFile: "6am8am.png", description: "Morning (6:00)" },
    {
      hour: 10,
      expectedFile: "10am12pm.png",
      description: "Late morning (10:00)",
    },
    { hour: 12, expectedFile: "12pm1am.png", description: "Noon (12:00)" },
    {
      hour: 14,
      expectedFile: "2pm4pm.png",
      description: "Afternoon (2:00 PM)",
    },
    { hour: 18, expectedFile: "6pm7pm.png", description: "Evening (6:00 PM)" },
    {
      hour: 23,
      expectedFile: "11pm12am.png",
      description: "Late night (11:00 PM)",
    },
  ];

  let passed = 0;
  let failed = 0;

  testCases.forEach(({ hour, expectedFile, description }) => {
    const entry = TIME_MAP.find((t) => hour >= t.range[0] && hour < t.range[1]);
    const selectedFile = entry
      ? `${IMAGE_BASE_PATH}${entry.file}`
      : FALLBACK_IMAGE;
    const expectedPath = `${IMAGE_BASE_PATH}${expectedFile}`;

    const isCorrect = selectedFile === expectedPath;
    const status = isCorrect ? "✅ PASS" : "❌ FAIL";

    console.log(`${status} | ${description}`);
    console.log(`   Hour: ${hour} → Expected: ${expectedPath}`);
    console.log(`   Got: ${selectedFile}\n`);

    isCorrect ? passed++ : failed++;
  });

  console.log(
    `\n📊 Results: ${passed} passed, ${failed} failed out of ${testCases.length} tests`,
  );
  return failed === 0;
}

testTimeBasedImageSelection();
