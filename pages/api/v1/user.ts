// Next.js API route support: https://nextjs.org/docs/api-routes/introduction
import type { NextApiRequest, NextApiResponse } from 'next';

interface ReqData {
  name: string;
  age: string;
}

export default function handler(req: NextApiRequest, res: NextApiResponse) {
  const { body, method } = req;

  res.status(200).json({ body, method });
}
